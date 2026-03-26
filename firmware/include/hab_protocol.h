#ifndef HAB_PROTOCOL_H
#define HAB_PROTOCOL_H

#include <stddef.h>

#include "hab_types.h"

const char *hab_state_name(hab_state_t state);
const char *hab_sd_csv_header(void);
int hab_format_lora_csv(const hab_runtime_t *runtime, char *buffer, size_t buffer_size);
int hab_format_sd_csv(const hab_runtime_t *runtime, char *buffer, size_t buffer_size);

#endif
