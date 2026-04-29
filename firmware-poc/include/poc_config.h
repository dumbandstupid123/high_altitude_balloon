#ifndef POC_CONFIG_H
#define POC_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

#define POC_I2C_SCAN_MIN_ADDRESS   0x08U
#define POC_I2C_SCAN_MAX_ADDRESS   0x77U
#define POC_BME680_ADDR_PRIMARY    0x76U
#define POC_BME680_ADDR_SECONDARY  0x77U

#define POC_LOOP_PERIOD_MS         1000U
#define POC_LORA_PERIOD_MS         5000U
#define POC_I2C_SCAN_PERIOD_MS     10000U

/* Compile-time test mode selector. Change only POC_TEST_MODE. */
#define POC_TEST_MODE_MICROSD      1U
#define POC_TEST_MODE_LORA_I2C     2U
#define POC_TEST_MODE_BME680       3U
#define POC_TEST_MODE_ALL          4U

/*
 * Active default mode for current bench bring-up:
 * LoRa + I2C scan enabled, microSD and BME680 disabled.
 */
#define POC_TEST_MODE              POC_TEST_MODE_LORA_I2C

#if (POC_TEST_MODE == POC_TEST_MODE_MICROSD)
#define POC_ENABLE_I2C_SCAN        0
#define POC_ENABLE_BME680          0
#define POC_ENABLE_LORA            0
#define POC_ENABLE_SD              1
#define POC_TEST_MODE_NAME         "MICROSD"
#elif (POC_TEST_MODE == POC_TEST_MODE_LORA_I2C)
#define POC_ENABLE_I2C_SCAN        1
#define POC_ENABLE_BME680          0
#define POC_ENABLE_LORA            1
#define POC_ENABLE_SD              0
#define POC_TEST_MODE_NAME         "LORA_I2C"
#elif (POC_TEST_MODE == POC_TEST_MODE_BME680)
#define POC_ENABLE_I2C_SCAN        1
#define POC_ENABLE_BME680          1
#define POC_ENABLE_LORA            0
#define POC_ENABLE_SD              0
#define POC_TEST_MODE_NAME         "BME680"
#elif (POC_TEST_MODE == POC_TEST_MODE_ALL)
#define POC_ENABLE_I2C_SCAN        1
#define POC_ENABLE_BME680          1
#define POC_ENABLE_LORA            1
#define POC_ENABLE_SD              1
#define POC_TEST_MODE_NAME         "ALL"
#else
#error "Invalid POC_TEST_MODE value"
#endif

#define POC_LORA_FREQUENCY_HZ      915000000UL
#define POC_LORA_BANDWIDTH_ENUM    7U
#define POC_LORA_SPREADING_FACTOR  9U
#define POC_LORA_CODING_RATE       1U
#define POC_LORA_PREAMBLE_SYMBOLS  8U
#define POC_LORA_TX_POWER_DBM      17
#define POC_LORA_TX_TIMEOUT_MS     1500U

#define POC_SD_LOG_PATH            "0:/POCLOG.CSV"

#define POC_I2C_TIMEOUT_CYCLES     200000U
#define POC_SPI_TIMEOUT_CYCLES     200000U

#endif
