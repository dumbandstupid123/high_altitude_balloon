#ifndef HAB_BME680_H
#define HAB_BME680_H

#include <stdbool.h>
#include <stdint.h>

#include "hab_types.h"
#include "bme68x.h"

typedef struct {
    void *context;
    int8_t (*read)(void *context, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t length);
    int8_t (*write)(void *context, uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint16_t length);
    void (*delay_us)(void *context, uint32_t delay_us);
    uint8_t i2c_address;
} hab_bme680_bus_t;

typedef struct {
    hab_bme680_bus_t bus;
    struct bme68x_dev dev;
    struct bme68x_conf conf;
    struct bme68x_heatr_conf heatr_conf;
    uint32_t measurement_delay_us;
    bool initialized;
} hab_bme680_t;

bool hab_bme680_init(hab_bme680_t *sensor, const hab_bme680_bus_t *bus);
bool hab_bme680_sample(hab_bme680_t *sensor, hab_env_data_t *env);

#endif
