#ifndef HAB_TYPES_H
#define HAB_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    HAB_STATE_BOOT = 0,
    HAB_STATE_SELF_TEST,
    HAB_STATE_GPS_ACQUIRE,
    HAB_STATE_FLIGHT_IDLE,
    HAB_STATE_FLIGHT_ACTIVE,
    HAB_STATE_EVENT_ACTIVE,
    HAB_STATE_FAULT
} hab_state_t;

enum {
    HAB_EVENT_NONE = 0u,
    HAB_EVENT_ASCENT_CONFIRMED = 1u << 0,
    HAB_EVENT_HIGH_ALTITUDE_TRIGGER = 1u << 1,
    HAB_EVENT_DESCENT_CONFIRMED = 1u << 2
};

enum {
    HAB_FAULT_NONE = 0u,
    HAB_FAULT_SELF_TEST = 1u << 0,
    HAB_FAULT_ENVIRONMENT_READ = 1u << 1,
    HAB_FAULT_GPS_STALE = 1u << 2,
    HAB_FAULT_SD_WRITE = 1u << 3,
    HAB_FAULT_LORA_TX = 1u << 4
};

typedef struct {
    bool valid_fix;
    bool valid_time;
    uint8_t satellites;
    char utc_time[16];
    double latitude_deg;
    double longitude_deg;
    float altitude_m;
} hab_gps_data_t;

typedef struct {
    bool valid;
    float temperature_c;
    float humidity_percent;
    float pressure_pa;
    float pressure_altitude_m;
    float filtered_altitude_m;
} hab_env_data_t;

typedef struct {
    hab_state_t state;
    uint32_t event_flags;
    uint32_t fault_flags;
    uint32_t telemetry_sequence;
    uint32_t log_sequence;
    uint32_t boot_seconds;
    bool launch_pressure_valid;
    float launch_pressure_pa;
    float peak_pressure_altitude_m;
    hab_gps_data_t gps;
    hab_env_data_t env;
} hab_runtime_t;

#endif
