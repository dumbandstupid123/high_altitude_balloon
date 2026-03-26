#ifdef HAB_HOST_SIM

#include "hab_platform.h"

#include <stdio.h>
#include <string.h>

#include "hab_math.h"
#include "hab_protocol.h"

typedef struct {
    FILE *log_file;
    unsigned int tick_count;
    float reference_pressure_pa;
} hab_stub_context_t;

static bool hab_stub_self_test(void *context, uint32_t *fault_flags)
{
    (void)context;
    (void)fault_flags;
    return true;
}

static float hab_stub_altitude_profile_m(unsigned int tick_count)
{
    if (tick_count < 120u) {
        return 20.0f * (float)tick_count;
    }

    return 2400.0f - (15.0f * (float)(tick_count - 120u));
}

static bool hab_stub_read_environment(void *context, hab_env_data_t *env)
{
    hab_stub_context_t *stub = (hab_stub_context_t *)context;
    float altitude_m = hab_stub_altitude_profile_m(stub->tick_count);

    env->temperature_c = 20.0f - (0.0065f * altitude_m);
    env->humidity_percent = 45.0f;
    env->pressure_pa = hab_pressure_from_altitude_m(altitude_m, stub->reference_pressure_pa);
    env->valid = true;

    stub->tick_count++;
    return true;
}

static bool hab_stub_log_csv_line(void *context, const char *line)
{
    hab_stub_context_t *stub = (hab_stub_context_t *)context;

    if (stub->log_file == NULL) {
        stub->log_file = fopen("hab_flight_log.csv", "w");
        if (stub->log_file == NULL) {
            return false;
        }

        fprintf(stub->log_file, "%s\n", hab_sd_csv_header());
    }

    return fprintf(stub->log_file, "%s\n", line) > 0;
}

static bool hab_stub_flush_log(void *context)
{
    hab_stub_context_t *stub = (hab_stub_context_t *)context;

    if (stub->log_file == NULL) {
        return true;
    }

    return fflush(stub->log_file) == 0;
}

static bool hab_stub_send_lora(void *context, const char *payload)
{
    (void)context;
    return fprintf(stdout, "%s\n", payload) > 0;
}

static void hab_stub_enter_sleep(void *context)
{
    (void)context;
}

void hab_platform_init_stub(hab_platform_t *platform)
{
    static hab_stub_context_t stub_context;

    memset(&stub_context, 0, sizeof(stub_context));
    stub_context.reference_pressure_pa = 101325.0f;

    memset(platform, 0, sizeof(*platform));
    platform->context = &stub_context;
    platform->self_test = hab_stub_self_test;
    platform->read_environment = hab_stub_read_environment;
    platform->log_csv_line = hab_stub_log_csv_line;
    platform->flush_log = hab_stub_flush_log;
    platform->send_lora = hab_stub_send_lora;
    platform->enter_sleep = hab_stub_enter_sleep;
}

#endif
