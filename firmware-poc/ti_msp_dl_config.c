#include "ti_msp_dl_config.h"

SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_UART_DEBUG_init();
    SYSCFG_DL_I2C_ENV_init();
    SYSCFG_DL_SPI_SHARED_init();
}

SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    return true;
}

SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    return true;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_UART_Main_reset(UART_DEBUG_INST);
    DL_I2C_reset(I2C_ENV_INST);
    DL_SPI_reset(SPI_SHARED_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_UART_Main_enablePower(UART_DEBUG_INST);
    DL_I2C_enablePower(I2C_ENV_INST);
    DL_SPI_enablePower(SPI_SHARED_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_DEBUG_IOMUX_TX, GPIO_UART_DEBUG_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_DEBUG_IOMUX_RX, GPIO_UART_DEBUG_IOMUX_RX_FUNC);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_ENV_IOMUX_SDA,
        GPIO_I2C_ENV_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_ENV_IOMUX_SCL,
        GPIO_I2C_ENV_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_ENV_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_ENV_IOMUX_SCL);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_SHARED_IOMUX_SCLK, GPIO_SPI_SHARED_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_SHARED_IOMUX_PICO, GPIO_SPI_SHARED_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_SPI_SHARED_IOMUX_POCI, GPIO_SPI_SHARED_IOMUX_POCI_FUNC);

    DL_GPIO_initDigitalOutput(GPIO_LORA_CS_IOMUX);
    DL_GPIO_initDigitalOutput(GPIO_LORA_RST_IOMUX);
    DL_GPIO_initDigitalInputFeatures(GPIO_LORA_DIO0_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalOutput(GPIO_SD_CS_IOMUX);

    DL_GPIO_setPins(GPIO_LORA_PORT, GPIO_LORA_CS_PIN | GPIO_LORA_RST_PIN);
    DL_GPIO_enableOutput(GPIO_LORA_PORT, GPIO_LORA_CS_PIN | GPIO_LORA_RST_PIN);

    DL_GPIO_setPins(GPIO_SD_PORT, GPIO_SD_CS_PIN);
    DL_GPIO_enableOutput(GPIO_SD_PORT, GPIO_SD_CS_PIN);
}

SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_disableHFXT();
    DL_SYSCTL_disableSYSPLL();
}

static const DL_UART_Main_ClockConfig g_uart_clock = {
    .clockSel = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1,
};

static const DL_UART_Main_Config g_uart_config = {
    .mode = DL_UART_MAIN_MODE_NORMAL,
    .direction = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity = DL_UART_MAIN_PARITY_NONE,
    .wordLength = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits = DL_UART_MAIN_STOP_BITS_ONE,
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_DEBUG_init(void)
{
    DL_UART_Main_setClockConfig(UART_DEBUG_INST, (DL_UART_Main_ClockConfig *)&g_uart_clock);
    DL_UART_Main_init(UART_DEBUG_INST, (DL_UART_Main_Config *)&g_uart_config);
    DL_UART_Main_setOversampling(UART_DEBUG_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_DEBUG_INST,
        UART_DEBUG_IBRD_32_MHZ_115200_BAUD,
        UART_DEBUG_FBRD_32_MHZ_115200_BAUD);
    DL_UART_Main_enable(UART_DEBUG_INST);
}

static const DL_I2C_ClockConfig g_i2c_clock = {
    .clockSel = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_I2C_ENV_init(void)
{
    DL_I2C_setClockConfig(I2C_ENV_INST, (DL_I2C_ClockConfig *)&g_i2c_clock);
    DL_I2C_disableAnalogGlitchFilter(I2C_ENV_INST);
    DL_I2C_resetControllerTransfer(I2C_ENV_INST);
    DL_I2C_setTimerPeriod(I2C_ENV_INST, 7);
    DL_I2C_setControllerTXFIFOThreshold(I2C_ENV_INST, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setControllerRXFIFOThreshold(I2C_ENV_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    DL_I2C_enableControllerClockStretching(I2C_ENV_INST);
    DL_I2C_enableController(I2C_ENV_INST);
}

static const DL_SPI_Config g_spi_config = {
    .mode = DL_SPI_MODE_CONTROLLER,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0,
    .parity = DL_SPI_PARITY_NONE,
    .dataSize = DL_SPI_DATA_SIZE_8,
    .bitOrder = DL_SPI_BIT_ORDER_MSB_FIRST,
    .chipSelectPin = DL_SPI_CHIP_SELECT_NONE,
};

static const DL_SPI_ClockConfig g_spi_clock = {
    .clockSel = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_SPI_SHARED_init(void)
{
    DL_SPI_setClockConfig(SPI_SHARED_INST, (DL_SPI_ClockConfig *)&g_spi_clock);
    DL_SPI_init(SPI_SHARED_INST, (DL_SPI_Config *)&g_spi_config);
    DL_SPI_setBitRateSerialClockDivider(SPI_SHARED_INST, 31);
    DL_SPI_setFIFOThreshold(SPI_SHARED_INST,
        DL_SPI_RX_FIFO_LEVEL_1_2_FULL,
        DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);
    DL_SPI_enable(SPI_SHARED_INST);
}
