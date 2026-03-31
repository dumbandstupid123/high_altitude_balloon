#include "poc_platform.h"

#include <stdarg.h>
#include <stdio.h>

#include "poc_config.h"
#include "ti_msp_dl_config.h"

static bool poc_i2c_wait_idle(void)
{
    uint32_t timeout = POC_I2C_TIMEOUT_CYCLES;

    while (timeout-- > 0u) {
        if ((DL_I2C_getControllerStatus(I2C_ENV_INST) & DL_I2C_CONTROLLER_STATUS_IDLE) != 0u) {
            return true;
        }
    }

    return false;
}

static bool poc_i2c_wait_complete(void)
{
    uint32_t timeout = POC_I2C_TIMEOUT_CYCLES;

    while (timeout-- > 0u) {
        uint32_t status = DL_I2C_getControllerStatus(I2C_ENV_INST);

        if ((status & DL_I2C_CONTROLLER_STATUS_ERROR) != 0u) {
            return false;
        }

        if ((status & DL_I2C_CONTROLLER_STATUS_BUSY_BUS) == 0u &&
            (status & DL_I2C_CONTROLLER_STATUS_IDLE) != 0u) {
            return true;
        }
    }

    return false;
}

void poc_platform_init(void)
{
    /* Drive both CS pins inactive before the first SPI transaction. */
    poc_lora_set_cs(false);
    poc_sd_set_cs(false);
    poc_lora_set_reset(false);
}

void poc_delay_us(uint32_t delay_us)
{
    if (delay_us == 0u) {
        return;
    }

    delay_cycles((CPUCLK_FREQ / 1000000u) * delay_us);
}

void poc_delay_ms(uint32_t delay_ms)
{
    while (delay_ms-- > 0u) {
        poc_delay_us(1000u);
    }
}

void poc_uart_write_char(char ch)
{
    DL_UART_Main_transmitDataBlocking(UART_DEBUG_INST, (uint8_t)ch);
}

void poc_uart_write_string(const char *text)
{
    size_t index = 0u;

    if (text == NULL) {
        return;
    }

    while (text[index] != '\0') {
        poc_uart_write_char(text[index]);
        index++;
    }
}

void poc_uart_write_line(const char *text)
{
    poc_uart_write_string(text);
    poc_uart_write_string("\r\n");
}

void poc_uart_printf(const char *format, ...)
{
    char buffer[192];
    va_list args;

    va_start(args, format);
    (void)vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    poc_uart_write_line(buffer);
}

bool poc_i2c_probe(uint8_t address)
{
    if (!poc_i2c_wait_idle()) {
        return false;
    }

    DL_I2C_resetControllerTransfer(I2C_ENV_INST);
    DL_I2C_startControllerTransferAdvanced(I2C_ENV_INST, address,
        DL_I2C_CONTROLLER_DIRECTION_TX, 0u,
        DL_I2C_CONTROLLER_START_ENABLE,
        DL_I2C_CONTROLLER_STOP_ENABLE,
        DL_I2C_CONTROLLER_ACK_DISABLE);

    poc_delay_us(50u);
    return poc_i2c_wait_complete();
}

int8_t poc_i2c_write_registers(uint8_t dev_addr,
                               uint8_t reg_addr,
                               const uint8_t *data,
                               uint16_t length)
{
    uint16_t index = 0u;

    if (!poc_i2c_wait_idle()) {
        return -1;
    }

    DL_I2C_flushControllerTXFIFO(I2C_ENV_INST);
    DL_I2C_resetControllerTransfer(I2C_ENV_INST);
    DL_I2C_startControllerTransfer(I2C_ENV_INST, dev_addr,
        DL_I2C_CONTROLLER_DIRECTION_TX, (uint16_t)(length + 1u));

    while (DL_I2C_isControllerTXFIFOFull(I2C_ENV_INST)) {
    }
    DL_I2C_transmitControllerData(I2C_ENV_INST, reg_addr);

    for (index = 0u; index < length; ++index) {
        while (DL_I2C_isControllerTXFIFOFull(I2C_ENV_INST)) {
        }
        DL_I2C_transmitControllerData(I2C_ENV_INST, data[index]);
    }

    return poc_i2c_wait_complete() ? 0 : -1;
}

int8_t poc_i2c_read_registers(uint8_t dev_addr,
                              uint8_t reg_addr,
                              uint8_t *data,
                              uint16_t length)
{
    uint16_t index = 0u;

    if (data == NULL || length == 0u) {
        return -1;
    }

    if (poc_i2c_write_registers(dev_addr, reg_addr, NULL, 0u) != 0) {
        return -1;
    }

    if (!poc_i2c_wait_idle()) {
        return -1;
    }

    DL_I2C_flushControllerRXFIFO(I2C_ENV_INST);
    DL_I2C_resetControllerTransfer(I2C_ENV_INST);
    DL_I2C_startControllerTransfer(I2C_ENV_INST, dev_addr,
        DL_I2C_CONTROLLER_DIRECTION_RX, length);

    for (index = 0u; index < length; ++index) {
        uint32_t timeout = POC_I2C_TIMEOUT_CYCLES;

        while (DL_I2C_isControllerRXFIFOEmpty(I2C_ENV_INST)) {
            if (timeout-- == 0u) {
                return -1;
            }
        }

        data[index] = DL_I2C_receiveControllerData(I2C_ENV_INST);
    }

    return poc_i2c_wait_complete() ? 0 : -1;
}

bool poc_spi_transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t length)
{
    size_t index = 0u;

    for (index = 0u; index < length; ++index) {
        uint8_t tx_byte = (tx_data != NULL) ? tx_data[index] : 0xFFu;

        DL_SPI_transmitDataBlocking8(SPI_SHARED_INST, tx_byte);
        if (rx_data != NULL) {
            rx_data[index] = DL_SPI_receiveDataBlocking8(SPI_SHARED_INST);
        } else {
            (void)DL_SPI_receiveDataBlocking8(SPI_SHARED_INST);
        }
    }

    return true;
}

uint8_t poc_spi_transfer_byte(uint8_t value)
{
    DL_SPI_transmitDataBlocking8(SPI_SHARED_INST, value);
    return DL_SPI_receiveDataBlocking8(SPI_SHARED_INST);
}

void poc_lora_set_cs(bool active)
{
    if (active) {
        DL_GPIO_clearPins(GPIO_LORA_PORT, GPIO_LORA_CS_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LORA_PORT, GPIO_LORA_CS_PIN);
    }
}

void poc_lora_set_reset(bool asserted)
{
    if (asserted) {
        DL_GPIO_clearPins(GPIO_LORA_PORT, GPIO_LORA_RST_PIN);
    } else {
        DL_GPIO_setPins(GPIO_LORA_PORT, GPIO_LORA_RST_PIN);
    }
}

bool poc_lora_read_dio0(void)
{
    return DL_GPIO_readPins(GPIO_LORA_PORT, GPIO_LORA_DIO0_PIN) != 0u;
}

bool poc_lora_read_register(uint8_t reg_addr, uint8_t *value_out)
{
    uint8_t tx_data[2] = {0};
    uint8_t rx_data[2] = {0};

    if (value_out == NULL) {
        return false;
    }

    tx_data[0] = reg_addr & 0x7Fu;
    tx_data[1] = 0xFFu;

    poc_lora_set_cs(true);
    (void)poc_spi_transfer(tx_data, rx_data, sizeof(tx_data));
    poc_lora_set_cs(false);

    *value_out = rx_data[1];
    return true;
}

void poc_sd_set_cs(bool active)
{
    if (active) {
        DL_GPIO_clearPins(GPIO_SD_PORT, GPIO_SD_CS_PIN);
    } else {
        DL_GPIO_setPins(GPIO_SD_PORT, GPIO_SD_CS_PIN);
    }
}
