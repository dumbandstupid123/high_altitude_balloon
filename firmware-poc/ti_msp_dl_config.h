/*
 * LaunchPad proof-of-concept configuration for LP-MSPM0G3507.
 * This file mirrors what SysConfig should generate from launchpad_poc.syscfg.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0G3507
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <stdbool.h>

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POWER_STARTUP_DELAY (16U)
#define CPUCLK_FREQ         (32000000U)

/* Debug UART over the XDS110 backchannel path on PA10/PA11. */
#define UART_DEBUG_INST                 UART0
#define UART_DEBUG_INST_FREQUENCY       32000000U
#define GPIO_UART_DEBUG_RX_PORT         GPIOA
#define GPIO_UART_DEBUG_TX_PORT         GPIOA
#define GPIO_UART_DEBUG_RX_PIN          DL_GPIO_PIN_11
#define GPIO_UART_DEBUG_TX_PIN          DL_GPIO_PIN_10
#define GPIO_UART_DEBUG_IOMUX_RX        (IOMUX_PINCM22)
#define GPIO_UART_DEBUG_IOMUX_TX        (IOMUX_PINCM21)
#define GPIO_UART_DEBUG_IOMUX_RX_FUNC   IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_DEBUG_IOMUX_TX_FUNC   IOMUX_PINCM21_PF_UART0_TX
#define UART_DEBUG_BAUD_RATE            (115200U)
#define UART_DEBUG_IBRD_32_MHZ_115200_BAUD (17U)
#define UART_DEBUG_FBRD_32_MHZ_115200_BAUD (23U)

/* LaunchPad-valid hardware I2C mapping for BME680 tests. */
#define I2C_ENV_INST                  I2C1
#define I2C_ENV_BUS_SPEED_HZ         (400000U)
#define GPIO_I2C_ENV_SDA_PORT        GPIOB
#define GPIO_I2C_ENV_SDA_PIN         DL_GPIO_PIN_3
#define GPIO_I2C_ENV_IOMUX_SDA       (IOMUX_PINCM16)
#define GPIO_I2C_ENV_IOMUX_SDA_FUNC  IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_I2C_ENV_SCL_PORT        GPIOB
#define GPIO_I2C_ENV_SCL_PIN         DL_GPIO_PIN_2
#define GPIO_I2C_ENV_IOMUX_SCL       (IOMUX_PINCM15)
#define GPIO_I2C_ENV_IOMUX_SCL_FUNC  IOMUX_PINCM15_PF_I2C1_SCL

/* Shared SPI bus for LoRa + microSD on LaunchPad pins. */
#define SPI_SHARED_INST                SPI1
#define GPIO_SPI_SHARED_PICO_PORT      GPIOB
#define GPIO_SPI_SHARED_PICO_PIN       DL_GPIO_PIN_8
#define GPIO_SPI_SHARED_IOMUX_PICO     (IOMUX_PINCM25)
#define GPIO_SPI_SHARED_IOMUX_PICO_FUNC IOMUX_PINCM25_PF_SPI1_PICO
#define GPIO_SPI_SHARED_POCI_PORT      GPIOB
#define GPIO_SPI_SHARED_POCI_PIN       DL_GPIO_PIN_7
#define GPIO_SPI_SHARED_IOMUX_POCI     (IOMUX_PINCM24)
#define GPIO_SPI_SHARED_IOMUX_POCI_FUNC IOMUX_PINCM24_PF_SPI1_POCI
#define GPIO_SPI_SHARED_SCLK_PORT      GPIOB
#define GPIO_SPI_SHARED_SCLK_PIN       DL_GPIO_PIN_9
#define GPIO_SPI_SHARED_IOMUX_SCLK     (IOMUX_PINCM26)
#define GPIO_SPI_SHARED_IOMUX_SCLK_FUNC IOMUX_PINCM26_PF_SPI1_SCLK

/* Manual chip-select and radio control pins. */
#define GPIO_LORA_PORT                GPIOB
#define GPIO_LORA_CS_PIN              DL_GPIO_PIN_6
#define GPIO_LORA_CS_IOMUX            (IOMUX_PINCM23)
#define GPIO_LORA_RST_PIN             DL_GPIO_PIN_0
#define GPIO_LORA_RST_IOMUX           (IOMUX_PINCM12)
#define GPIO_LORA_DIO0_PIN            DL_GPIO_PIN_15
#define GPIO_LORA_DIO0_IOMUX          (IOMUX_PINCM32)

#define GPIO_SD_PORT                  GPIOB
#define GPIO_SD_CS_PIN                DL_GPIO_PIN_17
#define GPIO_SD_CS_IOMUX              (IOMUX_PINCM43)

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_UART_DEBUG_init(void);
void SYSCFG_DL_I2C_ENV_init(void);
void SYSCFG_DL_SPI_SHARED_init(void);
bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif
