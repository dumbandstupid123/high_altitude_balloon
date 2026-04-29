#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hab_lora_sx1276.h"
#include "hab_sd_fatfs.h"
#include "hab_types.h"
#include "poc_config.h"
#include "poc_platform.h"
#include "ti_msp_dl_config.h"

#if POC_ENABLE_BME680
#include "hab_bme680.h"
#endif

#if POC_ENABLE_BME680
static hab_bme680_t g_bme680;
#endif
static hab_sx1276_t g_lora;
static hab_sd_fatfs_t g_sd;

#if POC_ENABLE_BME680
static bool g_bme680_ready = false;
#endif
static bool g_lora_ready = false;
static bool g_sd_ready = false;
#if POC_ENABLE_BME680
static uint8_t g_bme680_address = 0u;
#endif
static uint32_t g_uptime_ms = 0u;
static uint32_t g_lora_sequence = 0u;
static uint32_t g_sd_sequence = 0u;

#if POC_ENABLE_BME680
static int8_t poc_bme_i2c_read(void *context,
                               uint8_t dev_addr,
                               uint8_t reg_addr,
                               uint8_t *data,
                               uint16_t length)
{
    (void)context;
    return poc_i2c_read_registers(dev_addr, reg_addr, data, length);
}

static int8_t poc_bme_i2c_write(void *context,
                                uint8_t dev_addr,
                                uint8_t reg_addr,
                                const uint8_t *data,
                                uint16_t length)
{
    (void)context;
    return poc_i2c_write_registers(dev_addr, reg_addr, data, length);
}

static void poc_bme_delay_us(void *context, uint32_t delay_us)
{
    (void)context;
    poc_delay_us(delay_us);
}
#endif

static bool poc_lora_spi_transfer(void *context,
                                  const uint8_t *tx_data,
                                  uint8_t *rx_data,
                                  size_t length)
{
    (void)context;
    return poc_spi_transfer(tx_data, rx_data, length);
}

static void poc_lora_cs(void *context, bool active)
{
    (void)context;
    poc_lora_set_cs(active);
}

static void poc_lora_reset(void *context, bool asserted)
{
    (void)context;
    poc_lora_set_reset(asserted);
}

static bool poc_lora_irq(void *context)
{
    (void)context;
    return poc_lora_read_dio0();
}

static void poc_lora_delay_ms(void *context, uint32_t delay_ms)
{
    (void)context;
    poc_delay_ms(delay_ms);
}

static void poc_run_i2c_scan(void)
{
    uint8_t address = 0u;
    bool found_any = false;

    poc_uart_write_line("I2C scan start");

    for (address = POC_I2C_SCAN_MIN_ADDRESS; address <= POC_I2C_SCAN_MAX_ADDRESS; ++address) {
        if (poc_i2c_probe(address)) {
            poc_uart_printf("I2C found address 0x%02X", address);
            found_any = true;

#if POC_ENABLE_BME680
            if (address == POC_BME680_ADDR_PRIMARY || address == POC_BME680_ADDR_SECONDARY) {
                g_bme680_address = address;
            }
#endif
        }
    }

    if (!found_any) {
        poc_uart_write_line("I2C scan found no ACKed addresses");
    } else {
        poc_uart_write_line("I2C scan complete");
    }
}

#if POC_ENABLE_BME680
static void poc_init_bme680(void)
{
    hab_bme680_bus_t bus = {0};

    if (g_bme680_address == 0u) {
        if (poc_i2c_probe(POC_BME680_ADDR_PRIMARY)) {
            g_bme680_address = POC_BME680_ADDR_PRIMARY;
        } else if (poc_i2c_probe(POC_BME680_ADDR_SECONDARY)) {
            g_bme680_address = POC_BME680_ADDR_SECONDARY;
        }
    }

    if (g_bme680_address == 0u) {
        poc_uart_write_line("BME680 init skipped: no device found at 0x76/0x77");
        return;
    }

    bus.context = NULL;
    bus.read = poc_bme_i2c_read;
    bus.write = poc_bme_i2c_write;
    bus.delay_us = poc_bme_delay_us;
    bus.i2c_address = g_bme680_address;

    if (!hab_bme680_init(&g_bme680, &bus)) {
        poc_uart_printf("BME680 init failed at 0x%02X", g_bme680_address);
        return;
    }

    g_bme680_ready = true;
    poc_uart_printf("BME680 init OK at 0x%02X", g_bme680_address);
}

static void poc_sample_bme680(void)
{
    hab_env_data_t env = {0};

    if (!g_bme680_ready) {
        return;
    }

    if (!hab_bme680_sample(&g_bme680, &env)) {
        poc_uart_write_line("BME680 sample failed");
        return;
    }

    poc_uart_printf("BME680 temp=%.2fC hum=%.2f%% pressure=%.1fPa",
        (double)env.temperature_c,
        (double)env.humidity_percent,
        (double)env.pressure_pa);
}
#endif

static void poc_init_lora(void)
{
    hab_sx1276_hal_t hal = {0};
    hab_sx1276_config_t config = {0};
    uint8_t version = 0u;

    if (!poc_lora_read_register(0x42u, &version)) {
        poc_uart_write_line("LoRa version register read failed");
        return;
    }

    poc_uart_printf("LoRa version register = 0x%02X", version);

    hal.context = NULL;
    hal.spi_transfer = poc_lora_spi_transfer;
    hal.set_cs = poc_lora_cs;
    hal.set_reset = poc_lora_reset;
    hal.read_irq = poc_lora_irq;
    hal.delay_ms = poc_lora_delay_ms;

    config.frequency_hz = POC_LORA_FREQUENCY_HZ;
    config.bandwidth = POC_LORA_BANDWIDTH_ENUM;
    config.spreading_factor = POC_LORA_SPREADING_FACTOR;
    config.coding_rate = POC_LORA_CODING_RATE;
    config.preamble_symbols = POC_LORA_PREAMBLE_SYMBOLS;
    config.tx_power_dbm = POC_LORA_TX_POWER_DBM;
    config.crc_enabled = true;

    if (!hab_sx1276_init(&g_lora, &hal, &config)) {
        poc_uart_write_line("LoRa init failed");
        return;
    }

    g_lora_ready = true;
    poc_uart_write_line("LoRa init OK");
}

static void poc_send_lora_packet(void)
{
    char payload[96];

    if (!g_lora_ready) {
        return;
    }

    (void)snprintf(payload, sizeof(payload), "POC,%lu,%lu",
        (unsigned long)g_lora_sequence,
        (unsigned long)(g_uptime_ms / 1000u));

    if (hab_sx1276_send(&g_lora, (const uint8_t *)payload, strlen(payload), POC_LORA_TX_TIMEOUT_MS)) {
        poc_uart_printf("LoRa TX done: %s", payload);
        g_lora_sequence++;
    } else {
        poc_uart_write_line("LoRa TX failed");
    }
}

static void poc_init_sd(void)
{
    if (!hab_sd_fatfs_init(&g_sd, "", POC_SD_LOG_PATH)) {
        poc_uart_write_line("microSD init failed");
        return;
    }

    g_sd_ready = true;
    poc_uart_printf("microSD mount OK, log file %s", POC_SD_LOG_PATH);

    (void)hab_sd_fatfs_append_line(&g_sd, "seq,uptime_s,message");
    (void)hab_sd_fatfs_flush(&g_sd);
}

static void poc_log_sd_line(void)
{
    char line[96];

    if (!g_sd_ready) {
        return;
    }

    (void)snprintf(line, sizeof(line), "%lu,%lu,launchpad_poc_ok",
        (unsigned long)g_sd_sequence,
        (unsigned long)(g_uptime_ms / 1000u));

    if (hab_sd_fatfs_append_line(&g_sd, line) && hab_sd_fatfs_flush(&g_sd)) {
        poc_uart_printf("microSD wrote: %s", line);
        g_sd_sequence++;
    } else {
        poc_uart_write_line("microSD write failed");
    }
}

int main(void)
{
    uint32_t i2c_scan_elapsed_ms = POC_I2C_SCAN_PERIOD_MS;
    uint32_t lora_elapsed_ms = POC_LORA_PERIOD_MS;

    SYSCFG_DL_init();
    poc_platform_init();

    poc_uart_write_line("HAB LaunchPad firmware PoC");
    poc_uart_printf("Active test mode: %s", POC_TEST_MODE_NAME);

#if POC_ENABLE_I2C_SCAN
    poc_uart_write_line("I2C scan enabled");
    poc_run_i2c_scan();
#endif

#if POC_ENABLE_BME680
    poc_uart_write_line("BME680 test enabled");
    poc_init_bme680();
#endif

#if POC_ENABLE_LORA
    poc_uart_write_line("LoRa test enabled");
    poc_init_lora();
#endif

#if POC_ENABLE_SD
    poc_uart_write_line("microSD test enabled");
    poc_init_sd();
#endif

    while (1) {
        poc_delay_ms(POC_LOOP_PERIOD_MS);
        g_uptime_ms += POC_LOOP_PERIOD_MS;
        i2c_scan_elapsed_ms += POC_LOOP_PERIOD_MS;
        lora_elapsed_ms += POC_LOOP_PERIOD_MS;

#if POC_ENABLE_I2C_SCAN
        if (i2c_scan_elapsed_ms >= POC_I2C_SCAN_PERIOD_MS) {
            i2c_scan_elapsed_ms = 0u;
            poc_run_i2c_scan();
        }
#endif

#if POC_ENABLE_BME680
        poc_sample_bme680();
#endif

#if POC_ENABLE_SD
        poc_log_sd_line();
#endif

#if POC_ENABLE_LORA
        if (lora_elapsed_ms >= POC_LORA_PERIOD_MS) {
            lora_elapsed_ms = 0u;
            poc_send_lora_packet();
        }
#endif
    }
}
