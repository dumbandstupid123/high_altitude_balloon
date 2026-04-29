#include "hab_protocol.h"

#include <stdio.h>

const char *hab_state_name(hab_state_t state)
{
    switch (state) {
    case HAB_STATE_BOOT:
        return "BOOT";
    case HAB_STATE_SELF_TEST:
        return "SELF_TEST";
    case HAB_STATE_GPS_ACQUIRE:
        return "GPS_ACQUIRE";
    case HAB_STATE_FLIGHT_IDLE:
        return "FLIGHT_IDLE";
    case HAB_STATE_FLIGHT_ACTIVE:
        return "FLIGHT_ACTIVE";
    case HAB_STATE_EVENT_ACTIVE:
        return "EVENT_ACTIVE";
    case HAB_STATE_FAULT:
        return "FAULT";
    default:
        return "UNKNOWN";
    }
}

const char *hab_sd_csv_header(void)
{
    return "log_seq,boot_s,utc_time,gps_fix,sats,lat_deg,lon_deg,gps_alt_m,pressure_pa,pressure_alt_m,filtered_alt_m,temp_c,humidity_pct,state,event_flags,fault_flags";
}

int hab_format_lora_csv(const hab_runtime_t *runtime, char *buffer, size_t buffer_size)
{
    return snprintf(
        buffer,
        buffer_size,
        "HAB,%lu,%lu,%u,%u,%.6f,%.6f,%.1f,%.1f,%.1f,%.1f,%s,0x%02lX",
        (unsigned long)runtime->telemetry_sequence,
        (unsigned long)runtime->boot_seconds,
        runtime->gps.valid_fix ? 1u : 0u,
        (unsigned int)runtime->gps.satellites,
        runtime->gps.latitude_deg,
        runtime->gps.longitude_deg,
        runtime->gps.altitude_m,
        runtime->env.filtered_altitude_m,
        runtime->env.temperature_c,
        runtime->env.humidity_percent,
        hab_state_name(runtime->state),
        (unsigned long)runtime->event_flags);
}

int hab_format_sd_csv(const hab_runtime_t *runtime, char *buffer, size_t buffer_size)
{
    return snprintf(
        buffer,
        buffer_size,
        "%lu,%lu,%s,%u,%u,%.6f,%.6f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%s,0x%02lX,0x%02lX",
        (unsigned long)runtime->log_sequence,
        (unsigned long)runtime->boot_seconds,
        runtime->gps.valid_time ? runtime->gps.utc_time : "",
        runtime->gps.valid_fix ? 1u : 0u,
        (unsigned int)runtime->gps.satellites,
        runtime->gps.latitude_deg,
        runtime->gps.longitude_deg,
        runtime->gps.altitude_m,
        runtime->env.pressure_pa,
        runtime->env.pressure_altitude_m,
        runtime->env.filtered_altitude_m,
        runtime->env.temperature_c,
        runtime->env.humidity_percent,
        hab_state_name(runtime->state),
        (unsigned long)runtime->event_flags,
        (unsigned long)runtime->fault_flags);
}
