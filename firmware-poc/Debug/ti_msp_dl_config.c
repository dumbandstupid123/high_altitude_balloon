/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_SPI_backupConfig gSPI_SHAREDBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_I2C_ENV_init();
    SYSCFG_DL_UART_DEBUG_init();
    SYSCFG_DL_SPI_SHARED_init();
    /* Ensure backup structures have no valid state */

	gSPI_SHAREDBackup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_SPI_saveConfiguration(SPI_SHARED_INST, &gSPI_SHAREDBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_SPI_restoreConfiguration(SPI_SHARED_INST, &gSPI_SHAREDBackup);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_I2C_reset(I2C_ENV_INST);
    DL_UART_Main_reset(UART_DEBUG_INST);
    DL_SPI_reset(SPI_SHARED_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_I2C_enablePower(I2C_ENV_INST);
    DL_UART_Main_enablePower(UART_DEBUG_INST);
    DL_SPI_enablePower(SPI_SHARED_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{
    const uint8_t unusedPinIndexes[] =
    {
        IOMUX_PINCM30, IOMUX_PINCM31, IOMUX_PINCM33, IOMUX_PINCM34,
        IOMUX_PINCM35, IOMUX_PINCM36, IOMUX_PINCM37, IOMUX_PINCM38,
        IOMUX_PINCM39, IOMUX_PINCM40, IOMUX_PINCM44, IOMUX_PINCM45,
        IOMUX_PINCM46, IOMUX_PINCM47, IOMUX_PINCM48, IOMUX_PINCM49,
        IOMUX_PINCM50, IOMUX_PINCM51, IOMUX_PINCM52, IOMUX_PINCM53,
        IOMUX_PINCM54, IOMUX_PINCM55, IOMUX_PINCM56, IOMUX_PINCM57,
        IOMUX_PINCM58, IOMUX_PINCM59, IOMUX_PINCM60, IOMUX_PINCM1,
        IOMUX_PINCM2, IOMUX_PINCM3, IOMUX_PINCM4, IOMUX_PINCM5,
        IOMUX_PINCM6, IOMUX_PINCM8, IOMUX_PINCM9, IOMUX_PINCM10,
        IOMUX_PINCM11, IOMUX_PINCM13, IOMUX_PINCM14, IOMUX_PINCM17,
        IOMUX_PINCM18, IOMUX_PINCM19, IOMUX_PINCM20, IOMUX_PINCM27,
        IOMUX_PINCM28, IOMUX_PINCM29
    };

    for(int i = 0; i < sizeof(unusedPinIndexes)/sizeof(unusedPinIndexes[0]); i++)
    {
        DL_GPIO_initDigitalOutput(unusedPinIndexes[i]);
    }

    DL_GPIO_clearPins(GPIOA,
        (DL_GPIO_PIN_12 | DL_GPIO_PIN_13 | DL_GPIO_PIN_14 | DL_GPIO_PIN_15 |
        DL_GPIO_PIN_16 | DL_GPIO_PIN_17 | DL_GPIO_PIN_18 | DL_GPIO_PIN_21 |
        DL_GPIO_PIN_22 | DL_GPIO_PIN_23 | DL_GPIO_PIN_24 | DL_GPIO_PIN_25 |
        DL_GPIO_PIN_26 | DL_GPIO_PIN_27 | DL_GPIO_PIN_0 | DL_GPIO_PIN_1 |
        DL_GPIO_PIN_28 | DL_GPIO_PIN_29 | DL_GPIO_PIN_30 | DL_GPIO_PIN_31 |
        DL_GPIO_PIN_3 | DL_GPIO_PIN_4 | DL_GPIO_PIN_5 | DL_GPIO_PIN_6 |
        DL_GPIO_PIN_7 | DL_GPIO_PIN_8 | DL_GPIO_PIN_9));
    DL_GPIO_enableOutput(GPIOA,
        (DL_GPIO_PIN_12 | DL_GPIO_PIN_13 | DL_GPIO_PIN_14 | DL_GPIO_PIN_15 |
        DL_GPIO_PIN_16 | DL_GPIO_PIN_17 | DL_GPIO_PIN_18 | DL_GPIO_PIN_21 |
        DL_GPIO_PIN_22 | DL_GPIO_PIN_23 | DL_GPIO_PIN_24 | DL_GPIO_PIN_25 |
        DL_GPIO_PIN_26 | DL_GPIO_PIN_27 | DL_GPIO_PIN_0 | DL_GPIO_PIN_1 |
        DL_GPIO_PIN_28 | DL_GPIO_PIN_29 | DL_GPIO_PIN_30 | DL_GPIO_PIN_31 |
        DL_GPIO_PIN_3 | DL_GPIO_PIN_4 | DL_GPIO_PIN_5 | DL_GPIO_PIN_6 |
        DL_GPIO_PIN_7 | DL_GPIO_PIN_8 | DL_GPIO_PIN_9));
    DL_GPIO_clearPins(GPIOB,
        (DL_GPIO_PIN_13 | DL_GPIO_PIN_14 | DL_GPIO_PIN_16 | DL_GPIO_PIN_18 |
        DL_GPIO_PIN_19 | DL_GPIO_PIN_20 | DL_GPIO_PIN_21 | DL_GPIO_PIN_22 |
        DL_GPIO_PIN_23 | DL_GPIO_PIN_24 | DL_GPIO_PIN_25 | DL_GPIO_PIN_26 |
        DL_GPIO_PIN_27 | DL_GPIO_PIN_1 | DL_GPIO_PIN_4 | DL_GPIO_PIN_5 |
        DL_GPIO_PIN_10 | DL_GPIO_PIN_11 | DL_GPIO_PIN_12));
    DL_GPIO_enableOutput(GPIOB,
        (DL_GPIO_PIN_13 | DL_GPIO_PIN_14 | DL_GPIO_PIN_16 | DL_GPIO_PIN_18 |
        DL_GPIO_PIN_19 | DL_GPIO_PIN_20 | DL_GPIO_PIN_21 | DL_GPIO_PIN_22 |
        DL_GPIO_PIN_23 | DL_GPIO_PIN_24 | DL_GPIO_PIN_25 | DL_GPIO_PIN_26 |
        DL_GPIO_PIN_27 | DL_GPIO_PIN_1 | DL_GPIO_PIN_4 | DL_GPIO_PIN_5 |
        DL_GPIO_PIN_10 | DL_GPIO_PIN_11 | DL_GPIO_PIN_12));

    
	DL_GPIO_initPeripheralInputFunctionFeatures(
		 GPIO_I2C_ENV_IOMUX_SDA, GPIO_I2C_ENV_IOMUX_SDA_FUNC,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
	DL_GPIO_initPeripheralInputFunctionFeatures(
		 GPIO_I2C_ENV_IOMUX_SCL, GPIO_I2C_ENV_IOMUX_SCL_FUNC,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_ENV_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_ENV_IOMUX_SCL);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_DEBUG_IOMUX_TX, GPIO_UART_DEBUG_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_DEBUG_IOMUX_RX, GPIO_UART_DEBUG_IOMUX_RX_FUNC);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_SHARED_IOMUX_SCLK, GPIO_SPI_SHARED_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_SHARED_IOMUX_PICO, GPIO_SPI_SHARED_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_SPI_SHARED_IOMUX_POCI, GPIO_SPI_SHARED_IOMUX_POCI_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_SHARED_IOMUX_CS0, GPIO_SPI_SHARED_IOMUX_CS0_FUNC);

    DL_GPIO_initDigitalOutput(GPIO_LORA_LORA_CS_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_LORA_LORA_RST_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_LORA_LORA_DIO0_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_SD_SD_CS_IOMUX);

    DL_GPIO_setPins(GPIOB, GPIO_LORA_LORA_CS_PIN |
		GPIO_LORA_LORA_RST_PIN |
		GPIO_SD_SD_CS_PIN);
    DL_GPIO_enableOutput(GPIOB, GPIO_LORA_LORA_CS_PIN |
		GPIO_LORA_LORA_RST_PIN |
		GPIO_SD_SD_CS_PIN);

}



SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    
	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
	/* Set default configuration */
	DL_SYSCTL_disableHFXT();
	DL_SYSCTL_disableSYSPLL();

}


static const DL_I2C_ClockConfig gI2C_ENVClockConfig = {
    .clockSel = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_I2C_ENV_init(void) {

    DL_I2C_setClockConfig(I2C_ENV_INST,
        (DL_I2C_ClockConfig *) &gI2C_ENVClockConfig);
    DL_I2C_disableAnalogGlitchFilter(I2C_ENV_INST);

    /* Configure Controller Mode */
    DL_I2C_resetControllerTransfer(I2C_ENV_INST);
    /* Set frequency to 100000 Hz*/
    DL_I2C_setTimerPeriod(I2C_ENV_INST, 31);
    DL_I2C_setControllerTXFIFOThreshold(I2C_ENV_INST, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setControllerRXFIFOThreshold(I2C_ENV_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    DL_I2C_enableControllerClockStretching(I2C_ENV_INST);


    /* Enable module */
    DL_I2C_enableController(I2C_ENV_INST);


}

static const DL_UART_Main_ClockConfig gUART_DEBUGClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_DEBUGConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_DEBUG_init(void)
{
    DL_UART_Main_setClockConfig(UART_DEBUG_INST, (DL_UART_Main_ClockConfig *) &gUART_DEBUGClockConfig);

    DL_UART_Main_init(UART_DEBUG_INST, (DL_UART_Main_Config *) &gUART_DEBUGConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115211.52
     */
    DL_UART_Main_setOversampling(UART_DEBUG_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_DEBUG_INST, UART_DEBUG_IBRD_32_MHZ_115200_BAUD, UART_DEBUG_FBRD_32_MHZ_115200_BAUD);



    DL_UART_Main_enable(UART_DEBUG_INST);
}

static const DL_SPI_Config gSPI_SHARED_config = {
    .mode        = DL_SPI_MODE_CONTROLLER,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0,
    .parity      = DL_SPI_PARITY_NONE,
    .dataSize    = DL_SPI_DATA_SIZE_8,
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,
    .chipSelectPin = DL_SPI_CHIP_SELECT_0,
};

static const DL_SPI_ClockConfig gSPI_SHARED_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1
};

SYSCONFIG_WEAK void SYSCFG_DL_SPI_SHARED_init(void) {
    DL_SPI_setClockConfig(SPI_SHARED_INST, (DL_SPI_ClockConfig *) &gSPI_SHARED_clockConfig);

    DL_SPI_init(SPI_SHARED_INST, (DL_SPI_Config *) &gSPI_SHARED_config);

    /* Configure Controller mode */
    /*
     * Set the bit rate clock divider to generate the serial output clock
     *     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
     *     500000 = (32000000)/((1 + 31) * 2)
     */
    DL_SPI_setBitRateSerialClockDivider(SPI_SHARED_INST, 31);
    /* Set RX and TX FIFO threshold levels */
    DL_SPI_setFIFOThreshold(SPI_SHARED_INST, DL_SPI_RX_FIFO_LEVEL_1_2_FULL, DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);

    /* Enable module */
    DL_SPI_enable(SPI_SHARED_INST);
}

