#include "platform/hab_mspm0_ccs.h"

#include <stddef.h>
#include <string.h>

#include "hab_bme680.h"
#include "hab_lora_sx1276.h"
#include "hab_sd_fatfs.h"
#include "platform/mspm0_ccs_config.h"
#include "ti_msp_dl_config.h"

#define HAB_SOFT_I2C_DELAY_US 5U
#define HAB_SOFT_SPI_DELAY_US 1U

typedef struct {
    hab_bme680_t bme680;
    hab_sx1276_t radio;
    hab_sd_fatfs_t storage;
    bool low_level_ready;
} hab_mspm0_ccs_context_t;

static void hab_mspm0_delay_us(void *context, uint32_t delay_us);

static void hab_gpio_open_drain_low(GPIO_Regs *port, uint32_t pin)
{
    DL_GPIO_clearPins(port, pin);
    DL_GPIO_enableOutput(port, pin);
}

static void hab_gpio_open_drain_release(GPIO_Regs *port, uint32_t pin)
{
    DL_GPIO_disableOutput(port, pin);
}

static void hab_soft_i2c_release_sda(void)
{
    hab_gpio_open_drain_release(GPIO_AUX_BME680_SDA_PORT, GPIO_AUX_BME680_SDA_PIN);
}

static void hab_soft_i2c_pull_sda_low(void)
{
    hab_gpio_open_drain_low(GPIO_AUX_BME680_SDA_PORT, GPIO_AUX_BME680_SDA_PIN);
}

static void hab_soft_i2c_release_scl(void)
{
    hab_gpio_open_drain_release(GPIO_AUX_BME680_SCL_PORT, GPIO_AUX_BME680_SCL_PIN);
}

static void hab_soft_i2c_pull_scl_low(void)
{
    hab_gpio_open_drain_low(GPIO_AUX_BME680_SCL_PORT, GPIO_AUX_BME680_SCL_PIN);
}

static bool hab_soft_i2c_read_sda(void)
{
    return DL_GPIO_readPins(GPIO_AUX_BME680_SDA_PORT, GPIO_AUX_BME680_SDA_PIN) != 0u;
}

static void hab_soft_i2c_start(void *context)
{
    hab_soft_i2c_release_sda();
    hab_soft_i2c_release_scl();
    hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
    hab_soft_i2c_pull_sda_low();
    hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
    hab_soft_i2c_pull_scl_low();
}

static void hab_soft_i2c_stop(void *context)
{
    hab_soft_i2c_pull_sda_low();
    hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
    hab_soft_i2c_release_scl();
    hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
    hab_soft_i2c_release_sda();
    hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
}

static bool hab_soft_i2c_write_byte(void *context, uint8_t value)
{
    uint8_t bit = 0u;

    for (bit = 0u; bit < 8u; ++bit) {
        if ((value & 0x80u) != 0u) {
            hab_soft_i2c_release_sda();
        } else {
            hab_soft_i2c_pull_sda_low();
        }

        hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
        hab_soft_i2c_release_scl();
        hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
        hab_soft_i2c_pull_scl_low();
        value <<= 1;
    }

    hab_soft_i2c_release_sda();
    hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
    hab_soft_i2c_release_scl();
    hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
    bit = hab_soft_i2c_read_sda() ? 1u : 0u;
    hab_soft_i2c_pull_scl_low();
    return bit == 0u;
}

static uint8_t hab_soft_i2c_read_byte(void *context, bool ack)
{
    uint8_t bit = 0u;
    uint8_t value = 0u;

    hab_soft_i2c_release_sda();

    for (bit = 0u; bit < 8u; ++bit) {
        value <<= 1;
        hab_soft_i2c_release_scl();
        hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
        if (hab_soft_i2c_read_sda()) {
            value |= 1u;
        }
        hab_soft_i2c_pull_scl_low();
        hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
    }

    if (ack) {
        hab_soft_i2c_pull_sda_low();
    } else {
        hab_soft_i2c_release_sda();
    }

    hab_soft_i2c_release_scl();
    hab_mspm0_delay_us(context, HAB_SOFT_I2C_DELAY_US);
    hab_soft_i2c_pull_scl_low();
    hab_soft_i2c_release_sda();
    return value;
}

static int8_t hab_mspm0_i2c_write(void *context, uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint16_t length)
{
    uint16_t index = 0u;

    hab_soft_i2c_start(context);
    if (!hab_soft_i2c_write_byte(context, (uint8_t)(dev_addr << 1))) {
        hab_soft_i2c_stop(context);
        return -1;
    }

    if (!hab_soft_i2c_write_byte(context, reg_addr)) {
        hab_soft_i2c_stop(context);
        return -1;
    }

    for (index = 0u; index < length; ++index) {
        if (!hab_soft_i2c_write_byte(context, data[index])) {
            hab_soft_i2c_stop(context);
            return -1;
        }
    }

    hab_soft_i2c_stop(context);
    return 0;
}

static int8_t hab_mspm0_i2c_read(void *context, uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t length)
{
    uint16_t index = 0u;

    if (hab_mspm0_i2c_write(context, dev_addr, reg_addr, NULL, 0u) != 0) {
        return -1;
    }

    hab_soft_i2c_start(context);
    if (!hab_soft_i2c_write_byte(context, (uint8_t)((dev_addr << 1) | 1u))) {
        hab_soft_i2c_stop(context);
        return -1;
    }

    for (index = 0u; index < length; ++index) {
        data[index] = hab_soft_i2c_read_byte(context, index + 1u < length);
    }

    hab_soft_i2c_stop(context);
    return 0;
}

static void hab_mspm0_delay_us(void *context, uint32_t delay_us)
{
    (void)context;

    if (delay_us == 0u) {
        return;
    }

    delay_cycles((CPUCLK_FREQ / 1000000u) * delay_us);
}

static bool hab_mspm0_spi_transfer(void *context, const uint8_t *tx_data, uint8_t *rx_data, size_t length)
{
    size_t index = 0u;
    uint8_t bit = 0u;
    uint8_t tx_byte = 0u;
    uint8_t rx_byte = 0u;

    (void)context;

    for (index = 0u; index < length; ++index) {
        tx_byte = (tx_data != NULL) ? tx_data[index] : 0xFFu;
        rx_byte = 0u;

        for (bit = 0u; bit < 8u; ++bit) {
            if ((tx_byte & 0x80u) != 0u) {
                DL_GPIO_setPins(GPIO_AUX_SPI_MOSI_SOFT_PORT, GPIO_AUX_SPI_MOSI_SOFT_PIN);
            } else {
                DL_GPIO_clearPins(GPIO_AUX_SPI_MOSI_SOFT_PORT, GPIO_AUX_SPI_MOSI_SOFT_PIN);
            }

            hab_mspm0_delay_us(context, HAB_SOFT_SPI_DELAY_US);
            DL_GPIO_setPins(GPIO_AUX_SPI_CLK_SOFT_PORT, GPIO_AUX_SPI_CLK_SOFT_PIN);
            rx_byte <<= 1;
            if (DL_GPIO_readPins(GPIO_AUX_SPI_MISO_SOFT_PORT, GPIO_AUX_SPI_MISO_SOFT_PIN) != 0u) {
                rx_byte |= 1u;
            }
            hab_mspm0_delay_us(context, HAB_SOFT_SPI_DELAY_US);
            DL_GPIO_clearPins(GPIO_AUX_SPI_CLK_SOFT_PORT, GPIO_AUX_SPI_CLK_SOFT_PIN);
            tx_byte <<= 1;
        }

        if (rx_data != NULL) {
            rx_data[index] = rx_byte;
        }
    }

    return true;
}

static void hab_mspm0_lora_cs(void *context, bool active)
{
    (void)context;

    if (active) {
        DL_GPIO_clearPins(GPIO_LORA_PORT, GPIO_LORA_LORA_CS_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LORA_PORT, GPIO_LORA_LORA_CS_PIN);
    }
}

static void hab_mspm0_lora_reset(void *context, bool asserted)
{
    (void)context;

    if (asserted) {
        DL_GPIO_clearPins(GPIO_LORA_PORT, GPIO_LORA_LORA_RST_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LORA_PORT, GPIO_LORA_LORA_RST_PIN);
    }
}

static bool hab_mspm0_lora_irq(void *context)
{
    (void)context;
    return DL_GPIO_readPins(GPIO_LORA_PORT, GPIO_LORA_LORA_IRQ_PIN) != 0u;
}

static void hab_mspm0_delay_ms(void *context, uint32_t delay_ms)
{
    while (delay_ms-- > 0u) {
        hab_mspm0_delay_us(context, 1000u);
    }
}

static bool hab_mspm0_self_test(void *context, uint32_t *fault_flags)
{
    hab_mspm0_ccs_context_t *ctx = (hab_mspm0_ccs_context_t *)context;
    hab_bme680_bus_t bme_bus;
    hab_sx1276_hal_t radio_hal;
    hab_sx1276_config_t radio_cfg;

    if (ctx == NULL || fault_flags == NULL) {
        return false;
    }

    if (!ctx->low_level_ready) {
        *fault_flags |= HAB_FAULT_SELF_TEST;
        return false;
    }

    memset(&bme_bus, 0, sizeof(bme_bus));
    bme_bus.context = ctx;
    bme_bus.read = hab_mspm0_i2c_read;
    bme_bus.write = hab_mspm0_i2c_write;
    bme_bus.delay_us = hab_mspm0_delay_us;
    bme_bus.i2c_address = HAB_BME680_I2C_ADDRESS;

    if (!hab_bme680_init(&ctx->bme680, &bme_bus)) {
        *fault_flags |= HAB_FAULT_ENVIRONMENT_READ;
        return false;
    }

    memset(&radio_hal, 0, sizeof(radio_hal));
    radio_hal.context = ctx;
    radio_hal.spi_transfer = hab_mspm0_spi_transfer;
    radio_hal.set_cs = hab_mspm0_lora_cs;
    radio_hal.set_reset = hab_mspm0_lora_reset;
    radio_hal.read_irq = hab_mspm0_lora_irq;
    radio_hal.delay_ms = hab_mspm0_delay_ms;

    radio_cfg.frequency_hz = HAB_LORA_FREQUENCY_HZ;
    radio_cfg.bandwidth = HAB_LORA_BANDWIDTH_ENUM;
    radio_cfg.spreading_factor = HAB_LORA_SPREADING_FACTOR;
    radio_cfg.coding_rate = HAB_LORA_CODING_RATE;
    radio_cfg.preamble_symbols = HAB_LORA_PREAMBLE_SYMBOLS;
    radio_cfg.tx_power_dbm = HAB_LORA_TX_POWER_DBM;
    radio_cfg.crc_enabled = true;

    if (!hab_sx1276_init(&ctx->radio, &radio_hal, &radio_cfg)) {
        *fault_flags |= HAB_FAULT_LORA_TX;
        return false;
    }

    if (!hab_sd_fatfs_init(&ctx->storage, "", "0:/HABLOG.CSV")) {
        *fault_flags |= HAB_FAULT_SD_WRITE;
        return false;
    }

    return true;
}

static bool hab_mspm0_read_environment(void *context, hab_env_data_t *env)
{
    hab_mspm0_ccs_context_t *ctx = (hab_mspm0_ccs_context_t *)context;
    return hab_bme680_sample(&ctx->bme680, env);
}

static bool hab_mspm0_log_csv_line(void *context, const char *line)
{
    hab_mspm0_ccs_context_t *ctx = (hab_mspm0_ccs_context_t *)context;
    return hab_sd_fatfs_append_line(&ctx->storage, line);
}

static bool hab_mspm0_flush_log(void *context)
{
    hab_mspm0_ccs_context_t *ctx = (hab_mspm0_ccs_context_t *)context;
    return hab_sd_fatfs_flush(&ctx->storage);
}

static bool hab_mspm0_send_lora(void *context, const char *payload)
{
    hab_mspm0_ccs_context_t *ctx = (hab_mspm0_ccs_context_t *)context;
    const uint8_t *bytes = (const uint8_t *)payload;
    size_t length = 0u;

    while (payload[length] != '\0') {
        length++;
    }

    return hab_sx1276_send(&ctx->radio, bytes, length, HAB_LORA_TX_TIMEOUT_MS);
}

static void hab_mspm0_enter_sleep(void *context)
{
    (void)context;
}

bool hab_platform_init_mspm0_ccs(hab_platform_t *platform)
{
    static hab_mspm0_ccs_context_t context;

    if (platform == NULL) {
        return false;
    }

    memset(&context, 0, sizeof(context));
    context.low_level_ready = true;

    memset(platform, 0, sizeof(*platform));
    platform->context = &context;
    platform->self_test = hab_mspm0_self_test;
    platform->read_environment = hab_mspm0_read_environment;
    platform->log_csv_line = hab_mspm0_log_csv_line;
    platform->flush_log = hab_mspm0_flush_log;
    platform->send_lora = hab_mspm0_send_lora;
    platform->enter_sleep = hab_mspm0_enter_sleep;
    return true;
}
