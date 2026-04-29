#ifndef HAB_PLATFORM_H
#define HAB_PLATFORM_H

#include <stdbool.h>
#include <stdint.h>

#include "hab_types.h"

typedef struct {
    void *context;
    bool (*self_test)(void *context, uint32_t *fault_flags);
    bool (*read_environment)(void *context, hab_env_data_t *env);
    bool (*log_csv_line)(void *context, const char *line);
    bool (*flush_log)(void *context);
    bool (*send_lora)(void *context, const char *payload);
    void (*enter_sleep)(void *context);
} hab_platform_t;

void hab_platform_init_stub(hab_platform_t *platform);
bool hab_platform_init_mspm0_ccs(hab_platform_t *platform);

#endif
