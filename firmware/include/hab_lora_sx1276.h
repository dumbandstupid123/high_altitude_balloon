#ifndef HAB_LORA_SX1276_H
#define HAB_LORA_SX1276_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void *context;
    bool (*spi_transfer)(void *context, const uint8_t *tx_data, uint8_t *rx_data, size_t length);
    void (*set_cs)(void *context, bool active);
    void (*set_reset)(void *context, bool asserted);
    bool (*read_irq)(void *context);
    void (*delay_ms)(void *context, uint32_t delay_ms);
} hab_sx1276_hal_t;

typedef struct {
    uint32_t frequency_hz;
    uint8_t bandwidth;
    uint8_t spreading_factor;
    uint8_t coding_rate;
    uint16_t preamble_symbols;
    int8_t tx_power_dbm;
    bool crc_enabled;
} hab_sx1276_config_t;

typedef struct {
    hab_sx1276_hal_t hal;
    hab_sx1276_config_t config;
    bool initialized;
} hab_sx1276_t;

bool hab_sx1276_init(hab_sx1276_t *radio, const hab_sx1276_hal_t *hal, const hab_sx1276_config_t *config);
bool hab_sx1276_send(hab_sx1276_t *radio, const uint8_t *payload, size_t length, uint32_t timeout_ms);
bool hab_sx1276_sleep(hab_sx1276_t *radio);

#endif
