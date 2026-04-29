#include "hab_app.h"

#include <string.h>

#include "hab_config.h"
#include "hab_math.h"
#include "hab_protocol.h"

static void hab_set_state(hab_app_t *app, hab_state_t new_state)
{
    if (app->runtime.state != new_state) {
        app->runtime.state = new_state;
        app->telemetry_seconds_since_send = HAB_DEFAULT_TELEMETRY_PERIOD_S;
        app->log_records_since_flush = HAB_LOG_FLUSH_INTERVAL_RECORDS;
    }
}

static void hab_raise_fault(hab_app_t *app, uint32_t fault_flag, bool enter_fault_state)
{
    app->runtime.fault_flags |= fault_flag;

    if (enter_fault_state) {
        hab_set_state(app, HAB_STATE_FAULT);
    }
}

static void hab_update_environment(hab_app_t *app, const hab_platform_t *platform)
{
    hab_env_data_t env = {0};

    if (platform->read_environment == NULL) {
        return;
    }

    if (!platform->read_environment(platform->context, &env)) {
        hab_raise_fault(app, HAB_FAULT_ENVIRONMENT_READ, false);
        return;
    }

    env.valid = true;
    app->runtime.env = env;

    if (!app->runtime.launch_pressure_valid && env.pressure_pa > 0.0f) {
        app->runtime.launch_pressure_valid = true;
        app->runtime.launch_pressure_pa = env.pressure_pa;
    }

    if (!app->runtime.launch_pressure_valid) {
        return;
    }

    app->runtime.env.pressure_altitude_m =
        hab_pressure_altitude_m(app->runtime.env.pressure_pa, app->runtime.launch_pressure_pa);

    if (app->runtime.boot_seconds <= 1u) {
        app->runtime.env.filtered_altitude_m = app->runtime.env.pressure_altitude_m;
    } else {
        app->runtime.env.filtered_altitude_m = hab_low_pass_filter(
            app->runtime.env.filtered_altitude_m,
            app->runtime.env.pressure_altitude_m,
            HAB_ALTITUDE_FILTER_ALPHA);
    }

    if (app->runtime.env.filtered_altitude_m > app->runtime.peak_pressure_altitude_m) {
        app->runtime.peak_pressure_altitude_m = app->runtime.env.filtered_altitude_m;
    }
}

static bool hab_set_event_flag(hab_app_t *app, uint32_t flag)
{
    if ((app->runtime.event_flags & flag) != 0u) {
        return false;
    }

    app->runtime.event_flags |= flag;
    app->event_mode_seconds_remaining = HAB_EVENT_MODE_DWELL_S;
    hab_set_state(app, HAB_STATE_EVENT_ACTIVE);
    return true;
}

static void hab_update_events(hab_app_t *app)
{
    float altitude = app->runtime.env.filtered_altitude_m;

    if (!app->runtime.launch_pressure_valid || !app->runtime.env.valid) {
        return;
    }

    if (altitude >= HAB_ASCENT_CONFIRM_ALTITUDE_M) {
        app->ascent_dwell_seconds++;
    } else {
        app->ascent_dwell_seconds = 0u;
    }

    if (app->ascent_dwell_seconds >= HAB_ASCENT_CONFIRM_DWELL_S) {
        (void)hab_set_event_flag(app, HAB_EVENT_ASCENT_CONFIRMED);
    }

    if (altitude >= HAB_HIGH_ALTITUDE_TRIGGER_M) {
        (void)hab_set_event_flag(app, HAB_EVENT_HIGH_ALTITUDE_TRIGGER);
    }

    if ((app->runtime.event_flags & HAB_EVENT_ASCENT_CONFIRMED) != 0u &&
        (app->runtime.peak_pressure_altitude_m - altitude) >= HAB_DESCENT_CONFIRM_DROP_M) {
        (void)hab_set_event_flag(app, HAB_EVENT_DESCENT_CONFIRMED);
    }
}

static void hab_update_state_machine(hab_app_t *app)
{
    switch (app->runtime.state) {
    case HAB_STATE_BOOT:
    case HAB_STATE_SELF_TEST:
        hab_set_state(app, HAB_STATE_GPS_ACQUIRE);
        break;

    case HAB_STATE_GPS_ACQUIRE:
        app->gps_acquire_seconds++;
        if (app->runtime.gps.valid_fix || app->gps_acquire_seconds >= HAB_GPS_ACQUIRE_TIMEOUT_S) {
            hab_set_state(app, HAB_STATE_FLIGHT_IDLE);
        }
        break;

    case HAB_STATE_FLIGHT_IDLE:
        if ((app->runtime.event_flags & HAB_EVENT_ASCENT_CONFIRMED) != 0u ||
            app->runtime.env.filtered_altitude_m >= HAB_FLIGHT_IDLE_TO_ACTIVE_ALTITUDE_M) {
            hab_set_state(app, HAB_STATE_FLIGHT_ACTIVE);
        }
        break;

    case HAB_STATE_FLIGHT_ACTIVE:
        if (app->event_mode_seconds_remaining > 0u) {
            hab_set_state(app, HAB_STATE_EVENT_ACTIVE);
        }
        break;

    case HAB_STATE_EVENT_ACTIVE:
        if (app->event_mode_seconds_remaining > 0u) {
            app->event_mode_seconds_remaining--;
        } else {
            hab_set_state(app, HAB_STATE_FLIGHT_ACTIVE);
        }
        break;

    case HAB_STATE_FAULT:
    default:
        break;
    }
}

static uint32_t hab_telemetry_period_s(const hab_app_t *app)
{
    if (app->runtime.state == HAB_STATE_EVENT_ACTIVE) {
        return HAB_EVENT_TELEMETRY_PERIOD_S;
    }

    return HAB_DEFAULT_TELEMETRY_PERIOD_S;
}

static void hab_emit_log(hab_app_t *app, const hab_platform_t *platform)
{
    char line[HAB_SD_RECORD_MAX_LEN];

    if (platform->log_csv_line == NULL) {
        return;
    }

    app->runtime.log_sequence++;
    (void)hab_format_sd_csv(&app->runtime, line, sizeof(line));

    if (!platform->log_csv_line(platform->context, line)) {
        hab_raise_fault(app, HAB_FAULT_SD_WRITE, true);
        return;
    }

    app->log_records_since_flush++;
}

static void hab_emit_telemetry(hab_app_t *app, const hab_platform_t *platform)
{
    char packet[HAB_LORA_PACKET_MAX_LEN];

    if (platform->send_lora == NULL) {
        return;
    }

    app->runtime.telemetry_sequence++;
    (void)hab_format_lora_csv(&app->runtime, packet, sizeof(packet));

    if (!platform->send_lora(platform->context, packet)) {
        hab_raise_fault(app, HAB_FAULT_LORA_TX, true);
    }
}

static void hab_flush_if_needed(hab_app_t *app, const hab_platform_t *platform)
{
    if (platform->flush_log == NULL) {
        return;
    }

    if (app->log_records_since_flush < HAB_LOG_FLUSH_INTERVAL_RECORDS &&
        app->runtime.state != HAB_STATE_EVENT_ACTIVE &&
        app->runtime.state != HAB_STATE_FAULT) {
        return;
    }

    if (!platform->flush_log(platform->context)) {
        hab_raise_fault(app, HAB_FAULT_SD_WRITE, true);
        return;
    }

    app->log_records_since_flush = 0u;
}

void hab_app_init(hab_app_t *app)
{
    memset(app, 0, sizeof(*app));
    app->runtime.state = HAB_STATE_BOOT;
    hab_gps_parser_init(&app->gps_parser);
}

bool hab_app_bootstrap(hab_app_t *app, const hab_platform_t *platform)
{
    uint32_t fault_flags = 0u;

    if (app == NULL || platform == NULL) {
        return false;
    }

    hab_set_state(app, HAB_STATE_SELF_TEST);

    if (platform->self_test != NULL && !platform->self_test(platform->context, &fault_flags)) {
        app->runtime.fault_flags |= fault_flags | HAB_FAULT_SELF_TEST;
        hab_set_state(app, HAB_STATE_FAULT);
        return true;
    }

    hab_set_state(app, HAB_STATE_GPS_ACQUIRE);
    return true;
}

void hab_app_on_gps_byte(hab_app_t *app, char byte)
{
    (void)hab_gps_parser_consume(&app->gps_parser, byte, &app->runtime.gps);
}

void hab_app_on_tick(hab_app_t *app, const hab_platform_t *platform)
{
    app->runtime.boot_seconds++;
    app->telemetry_seconds_since_send++;

    hab_update_environment(app, platform);
    hab_update_events(app);
    hab_update_state_machine(app);

    hab_emit_log(app, platform);

    if (app->telemetry_seconds_since_send >= hab_telemetry_period_s(app)) {
        hab_emit_telemetry(app, platform);
        app->telemetry_seconds_since_send = 0u;
    }

    hab_flush_if_needed(app, platform);

    if (platform->enter_sleep != NULL) {
        platform->enter_sleep(platform->context);
    }
}

const hab_runtime_t *hab_app_runtime(const hab_app_t *app)
{
    return &app->runtime;
}
