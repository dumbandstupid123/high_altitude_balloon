#ifndef POC_PLATFORM_H
#define POC_PLATFORM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void poc_platform_init(void);
void poc_delay_us(uint32_t delay_us);
void poc_delay_ms(uint32_t delay_ms);

void poc_uart_write_char(char ch);
void poc_uart_write_string(const char *text);
void poc_uart_write_line(const char *text);
void poc_uart_printf(const char *format, ...);

bool poc_i2c_probe(uint8_t address);
int8_t poc_i2c_write_registers(uint8_t dev_addr,
                               uint8_t reg_addr,
                               const uint8_t *data,
                               uint16_t length);
int8_t poc_i2c_read_registers(uint8_t dev_addr,
                              uint8_t reg_addr,
                              uint8_t *data,
                              uint16_t length);

bool poc_spi_transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t length);
uint8_t poc_spi_transfer_byte(uint8_t value);

void poc_lora_set_cs(bool active);
void poc_lora_set_reset(bool asserted);
bool poc_lora_read_dio0(void);
bool poc_lora_read_register(uint8_t reg_addr, uint8_t *value_out);

void poc_sd_set_cs(bool active);

#endif
