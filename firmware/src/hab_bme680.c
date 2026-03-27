#include "hab_bme680.h"

#include <string.h>

static BME68X_INTF_RET_TYPE hab_bme680_bus_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    hab_bme680_bus_t *bus = (hab_bme680_bus_t *)intf_ptr;

    if (bus == NULL || bus->read == NULL) {
        return BME68X_E_NULL_PTR;
    }

    return bus->read(bus->context, bus->i2c_address, reg_addr, reg_data, (uint16_t)length);
}

static BME68X_INTF_RET_TYPE hab_bme680_bus_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr)
{
    hab_bme680_bus_t *bus = (hab_bme680_bus_t *)intf_ptr;

    if (bus == NULL || bus->write == NULL) {
        return BME68X_E_NULL_PTR;
    }

    return bus->write(bus->context, bus->i2c_address, reg_addr, reg_data, (uint16_t)length);
}

static void hab_bme680_bus_delay(uint32_t period, void *intf_ptr)
{
    hab_bme680_bus_t *bus = (hab_bme680_bus_t *)intf_ptr;

    if (bus != NULL && bus->delay_us != NULL) {
        bus->delay_us(bus->context, period);
    }
}

bool hab_bme680_init(hab_bme680_t *sensor, const hab_bme680_bus_t *bus)
{
    int8_t result = BME68X_OK;

    if (sensor == NULL || bus == NULL) {
        return false;
    }

    memset(sensor, 0, sizeof(*sensor));
    sensor->bus = *bus;

    sensor->dev.intf = BME68X_I2C_INTF;
    sensor->dev.intf_ptr = &sensor->bus;
    sensor->dev.read = hab_bme680_bus_read;
    sensor->dev.write = hab_bme680_bus_write;
    sensor->dev.delay_us = hab_bme680_bus_delay;
    sensor->dev.amb_temp = 25;

    result = bme68x_init(&sensor->dev);
    if (result != BME68X_OK) {
        return false;
    }

    sensor->conf.filter = BME68X_FILTER_SIZE_3;
    sensor->conf.odr = BME68X_ODR_NONE;
    sensor->conf.os_hum = BME68X_OS_2X;
    sensor->conf.os_pres = BME68X_OS_4X;
    sensor->conf.os_temp = BME68X_OS_8X;

    result = bme68x_set_conf(&sensor->conf, &sensor->dev);
    if (result != BME68X_OK) {
        return false;
    }

    sensor->heatr_conf.enable = BME68X_DISABLE;
    sensor->heatr_conf.heatr_temp = 0u;
    sensor->heatr_conf.heatr_dur = 0u;

    result = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &sensor->heatr_conf, &sensor->dev);
    if (result != BME68X_OK) {
        return false;
    }

    sensor->measurement_delay_us = bme68x_get_meas_dur(BME68X_FORCED_MODE, &sensor->conf, &sensor->dev);
    sensor->initialized = true;
    return true;
}

bool hab_bme680_sample(hab_bme680_t *sensor, hab_env_data_t *env)
{
    struct bme68x_data sample = {0};
    uint8_t sample_count = 0u;
    int8_t result = BME68X_OK;

    if (sensor == NULL || env == NULL || !sensor->initialized) {
        return false;
    }

    result = bme68x_set_op_mode(BME68X_FORCED_MODE, &sensor->dev);
    if (result != BME68X_OK) {
        return false;
    }

    if (sensor->bus.delay_us != NULL) {
        sensor->bus.delay_us(sensor->bus.context, sensor->measurement_delay_us + 5000u);
    }

    result = bme68x_get_data(BME68X_FORCED_MODE, &sample, &sample_count, &sensor->dev);
    if (result != BME68X_OK && result != BME68X_W_NO_NEW_DATA) {
        return false;
    }

    if (sample_count == 0u || (sample.status & BME68X_NEW_DATA_MSK) == 0u) {
        return false;
    }

    env->temperature_c = sample.temperature;
    env->pressure_pa = sample.pressure;
    env->humidity_percent = sample.humidity;
    env->valid = true;
    return true;
}
