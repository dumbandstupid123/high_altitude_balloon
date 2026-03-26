#ifndef HAB_APP_H
#define HAB_APP_H

#include <stdbool.h>
#include <stdint.h>

#include "hab_gps.h"
#include "hab_platform.h"
#include "hab_types.h"

typedef struct {
    hab_runtime_t runtime;
    hab_gps_parser_t gps_parser;
    uint32_t gps_acquire_seconds;
    uint32_t ascent_dwell_seconds;
    uint32_t event_mode_seconds_remaining;
    uint32_t log_records_since_flush;
    uint32_t telemetry_seconds_since_send;
} hab_app_t;

void hab_app_init(hab_app_t *app);
bool hab_app_bootstrap(hab_app_t *app, const hab_platform_t *platform);
void hab_app_on_gps_byte(hab_app_t *app, char byte);
void hab_app_on_tick(hab_app_t *app, const hab_platform_t *platform);
const hab_runtime_t *hab_app_runtime(const hab_app_t *app);

#endif
