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
    SYSCFG_DL_TIMER_SCHED_init();
    SYSCFG_DL_UART_DEBUG_init();
}



SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerG_reset(TIMER_SCHED_INST);
    DL_UART_Main_reset(UART_DEBUG_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerG_enablePower(TIMER_SCHED_INST);
    DL_UART_Main_enablePower(UART_DEBUG_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{
    const uint8_t unusedPinIndexes[] =
    {
        IOMUX_PINCM30, IOMUX_PINCM31, IOMUX_PINCM32, IOMUX_PINCM33,
        IOMUX_PINCM40, IOMUX_PINCM43, IOMUX_PINCM44, IOMUX_PINCM45,
        IOMUX_PINCM46, IOMUX_PINCM47, IOMUX_PINCM48, IOMUX_PINCM49,
        IOMUX_PINCM50, IOMUX_PINCM51, IOMUX_PINCM52, IOMUX_PINCM53,
        IOMUX_PINCM54, IOMUX_PINCM55, IOMUX_PINCM56, IOMUX_PINCM57,
        IOMUX_PINCM58, IOMUX_PINCM60, IOMUX_PINCM1, IOMUX_PINCM2,
        IOMUX_PINCM3, IOMUX_PINCM4, IOMUX_PINCM5, IOMUX_PINCM6,
        IOMUX_PINCM7, IOMUX_PINCM8, IOMUX_PINCM9, IOMUX_PINCM10,
        IOMUX_PINCM11, IOMUX_PINCM12, IOMUX_PINCM13, IOMUX_PINCM14,
        IOMUX_PINCM17, IOMUX_PINCM18, IOMUX_PINCM23, IOMUX_PINCM24,
        IOMUX_PINCM25, IOMUX_PINCM26, IOMUX_PINCM27, IOMUX_PINCM28,
        IOMUX_PINCM29
    };

    for(int i = 0; i < sizeof(unusedPinIndexes)/sizeof(unusedPinIndexes[0]); i++)
    {
        DL_GPIO_initDigitalOutput(unusedPinIndexes[i]);
    }

    DL_GPIO_clearPins(GPIOA,
        (DL_GPIO_PIN_18 | DL_GPIO_PIN_21 | DL_GPIO_PIN_22 | DL_GPIO_PIN_23 |
        DL_GPIO_PIN_24 | DL_GPIO_PIN_25 | DL_GPIO_PIN_27 | DL_GPIO_PIN_0 |
        DL_GPIO_PIN_1 | DL_GPIO_PIN_28 | DL_GPIO_PIN_29 | DL_GPIO_PIN_30 |
        DL_GPIO_PIN_31 | DL_GPIO_PIN_2 | DL_GPIO_PIN_3 | DL_GPIO_PIN_4 |
        DL_GPIO_PIN_5 | DL_GPIO_PIN_6 | DL_GPIO_PIN_7));
    DL_GPIO_enableOutput(GPIOA,
        (DL_GPIO_PIN_18 | DL_GPIO_PIN_21 | DL_GPIO_PIN_22 | DL_GPIO_PIN_23 |
        DL_GPIO_PIN_24 | DL_GPIO_PIN_25 | DL_GPIO_PIN_27 | DL_GPIO_PIN_0 |
        DL_GPIO_PIN_1 | DL_GPIO_PIN_28 | DL_GPIO_PIN_29 | DL_GPIO_PIN_30 |
        DL_GPIO_PIN_31 | DL_GPIO_PIN_2 | DL_GPIO_PIN_3 | DL_GPIO_PIN_4 |
        DL_GPIO_PIN_5 | DL_GPIO_PIN_6 | DL_GPIO_PIN_7));
    DL_GPIO_clearPins(GPIOB,
        (DL_GPIO_PIN_13 | DL_GPIO_PIN_14 | DL_GPIO_PIN_15 | DL_GPIO_PIN_16 |
        DL_GPIO_PIN_17 | DL_GPIO_PIN_18 | DL_GPIO_PIN_19 | DL_GPIO_PIN_20 |
        DL_GPIO_PIN_21 | DL_GPIO_PIN_22 | DL_GPIO_PIN_23 | DL_GPIO_PIN_24 |
        DL_GPIO_PIN_25 | DL_GPIO_PIN_26 | DL_GPIO_PIN_27 | DL_GPIO_PIN_0 |
        DL_GPIO_PIN_1 | DL_GPIO_PIN_4 | DL_GPIO_PIN_5 | DL_GPIO_PIN_6 |
        DL_GPIO_PIN_7 | DL_GPIO_PIN_8 | DL_GPIO_PIN_9 | DL_GPIO_PIN_10 |
        DL_GPIO_PIN_11 | DL_GPIO_PIN_12));
    DL_GPIO_enableOutput(GPIOB,
        (DL_GPIO_PIN_13 | DL_GPIO_PIN_14 | DL_GPIO_PIN_15 | DL_GPIO_PIN_16 |
        DL_GPIO_PIN_17 | DL_GPIO_PIN_18 | DL_GPIO_PIN_19 | DL_GPIO_PIN_20 |
        DL_GPIO_PIN_21 | DL_GPIO_PIN_22 | DL_GPIO_PIN_23 | DL_GPIO_PIN_24 |
        DL_GPIO_PIN_25 | DL_GPIO_PIN_26 | DL_GPIO_PIN_27 | DL_GPIO_PIN_0 |
        DL_GPIO_PIN_1 | DL_GPIO_PIN_4 | DL_GPIO_PIN_5 | DL_GPIO_PIN_6 |
        DL_GPIO_PIN_7 | DL_GPIO_PIN_8 | DL_GPIO_PIN_9 | DL_GPIO_PIN_10 |
        DL_GPIO_PIN_11 | DL_GPIO_PIN_12));

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_DEBUG_IOMUX_TX, GPIO_UART_DEBUG_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_DEBUG_IOMUX_RX, GPIO_UART_DEBUG_IOMUX_RX_FUNC);

    DL_GPIO_initDigitalOutput(GPIO_LORA_LORA_CS_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_LORA_LORA_IRQ_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_LORA_LORA_RST_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_AUX_SD_CS_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_AUX_BME680_SDA_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_AUX_BME680_SCL_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_AUX_SPI_CLK_SOFT_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_AUX_SPI_MOSI_SOFT_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_AUX_SPI_MISO_SOFT_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_AUX_GPS_RX_SOFT_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_AUX_GPS_TX_UNUSED_IOMUX);

    DL_GPIO_clearPins(GPIOA, GPIO_AUX_SPI_CLK_SOFT_PIN |
		GPIO_AUX_SPI_MOSI_SOFT_PIN);
    DL_GPIO_setPins(GPIOA, GPIO_LORA_LORA_CS_PIN |
		GPIO_LORA_LORA_RST_PIN |
		GPIO_AUX_SD_CS_PIN |
		GPIO_AUX_GPS_TX_UNUSED_PIN);
    DL_GPIO_enableOutput(GPIOA, GPIO_LORA_LORA_CS_PIN |
		GPIO_LORA_LORA_RST_PIN |
		GPIO_AUX_SD_CS_PIN |
		GPIO_AUX_SPI_CLK_SOFT_PIN |
		GPIO_AUX_SPI_MOSI_SOFT_PIN |
		GPIO_AUX_GPS_TX_UNUSED_PIN);
    DL_GPIO_setLowerPinsPolarity(GPIOA, DL_GPIO_PIN_10_EDGE_FALL);
    DL_GPIO_setLowerPinsInputFilter(GPIOA, DL_GPIO_PIN_10_INPUT_FILTER_3_CYCLES);
    DL_GPIO_clearInterruptStatus(GPIOA, GPIO_AUX_GPS_RX_SOFT_PIN);
    DL_GPIO_enableInterrupt(GPIOA, GPIO_AUX_GPS_RX_SOFT_PIN);

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



/*
 * Timer clock configuration to be sourced by BUSCLK /  (4000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   31250 Hz = 4000000 Hz / (8 * (127 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_SCHEDClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale    = 127U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_SCHED_INST_LOAD_VALUE = (1 s * 31250 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMER_SCHEDTimerConfig = {
    .period     = TIMER_SCHED_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_SCHED_init(void) {

    DL_TimerG_setClockConfig(TIMER_SCHED_INST,
        (DL_TimerG_ClockConfig *) &gTIMER_SCHEDClockConfig);

    DL_TimerG_initTimerMode(TIMER_SCHED_INST,
        (DL_TimerG_TimerConfig *) &gTIMER_SCHEDTimerConfig);
    DL_TimerG_enableInterrupt(TIMER_SCHED_INST , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMER_SCHED_INST);





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


    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART_DEBUG_INST);
    DL_UART_Main_setRXFIFOThreshold(UART_DEBUG_INST, DL_UART_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_UART_Main_setTXFIFOThreshold(UART_DEBUG_INST, DL_UART_TX_FIFO_LEVEL_1_2_EMPTY);

    DL_UART_Main_enableLoopbackMode(UART_DEBUG_INST);

    DL_UART_Main_enable(UART_DEBUG_INST);
}

