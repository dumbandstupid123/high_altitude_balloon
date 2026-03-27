#include "hab_lora_sx1276.h"

#define SX1276_REG_FIFO 0x00U
#define SX1276_REG_OP_MODE 0x01U
#define SX1276_REG_FRF_MSB 0x06U
#define SX1276_REG_FRF_MID 0x07U
#define SX1276_REG_FRF_LSB 0x08U
#define SX1276_REG_PA_CONFIG 0x09U
#define SX1276_REG_OCP 0x0BU
#define SX1276_REG_LNA 0x0CU
#define SX1276_REG_FIFO_ADDR_PTR 0x0DU
#define SX1276_REG_FIFO_TX_BASE_ADDR 0x0EU
#define SX1276_REG_IRQ_FLAGS 0x12U
#define SX1276_REG_MODEM_CONFIG1 0x1DU
#define SX1276_REG_MODEM_CONFIG2 0x1EU
#define SX1276_REG_PREAMBLE_MSB 0x20U
#define SX1276_REG_PREAMBLE_LSB 0x21U
#define SX1276_REG_PAYLOAD_LENGTH 0x22U
#define SX1276_REG_MODEM_CONFIG3 0x26U
#define SX1276_REG_DIO_MAPPING1 0x40U
#define SX1276_REG_VERSION 0x42U
#define SX1276_REG_PA_DAC 0x4DU

#define SX1276_LONG_RANGE_MODE 0x80U
#define SX1276_MODE_SLEEP 0x00U
#define SX1276_MODE_STDBY 0x01U
#define SX1276_MODE_TX 0x03U

#define SX1276_IRQ_TX_DONE 0x08U
#define SX1276_PA_BOOST 0x80U

static bool hab_sx1276_transfer(hab_sx1276_t *radio, const uint8_t *tx_data, uint8_t *rx_data, size_t length)
{
    if (radio == NULL || radio->hal.spi_transfer == NULL || radio->hal.set_cs == NULL) {
        return false;
    }

    radio->hal.set_cs(radio->hal.context, true);
    if (!radio->hal.spi_transfer(radio->hal.context, tx_data, rx_data, length)) {
        radio->hal.set_cs(radio->hal.context, false);
        return false;
    }
    radio->hal.set_cs(radio->hal.context, false);

    return true;
}

static bool hab_sx1276_write_reg(hab_sx1276_t *radio, uint8_t reg, uint8_t value)
{
    uint8_t tx_data[2];

    tx_data[0] = reg | 0x80U;
    tx_data[1] = value;
    return hab_sx1276_transfer(radio, tx_data, NULL, sizeof(tx_data));
}

static bool hab_sx1276_read_reg(hab_sx1276_t *radio, uint8_t reg, uint8_t *value)
{
    uint8_t tx_data[2] = {0};
    uint8_t rx_data[2] = {0};

    tx_data[0] = reg & 0x7FU;

    if (!hab_sx1276_transfer(radio, tx_data, rx_data, sizeof(tx_data))) {
        return false;
    }

    *value = rx_data[1];
    return true;
}

static bool hab_sx1276_write_burst(hab_sx1276_t *radio, uint8_t reg, const uint8_t *data, size_t length)
{
    size_t index = 0u;
    uint8_t tx_byte = reg | 0x80U;

    if (radio == NULL || data == NULL || radio->hal.spi_transfer == NULL || radio->hal.set_cs == NULL) {
        return false;
    }

    radio->hal.set_cs(radio->hal.context, true);
    if (!radio->hal.spi_transfer(radio->hal.context, &tx_byte, NULL, 1u)) {
        radio->hal.set_cs(radio->hal.context, false);
        return false;
    }

    for (index = 0u; index < length; ++index) {
        if (!radio->hal.spi_transfer(radio->hal.context, &data[index], NULL, 1u)) {
            radio->hal.set_cs(radio->hal.context, false);
            return false;
        }
    }

    radio->hal.set_cs(radio->hal.context, false);
    return true;
}

static bool hab_sx1276_set_frequency(hab_sx1276_t *radio, uint32_t frequency_hz)
{
    uint64_t frf = ((uint64_t)frequency_hz << 19) / 32000000ULL;

    return hab_sx1276_write_reg(radio, SX1276_REG_FRF_MSB, (uint8_t)(frf >> 16)) &&
           hab_sx1276_write_reg(radio, SX1276_REG_FRF_MID, (uint8_t)(frf >> 8)) &&
           hab_sx1276_write_reg(radio, SX1276_REG_FRF_LSB, (uint8_t)frf);
}

static bool hab_sx1276_set_op_mode(hab_sx1276_t *radio, uint8_t mode)
{
    return hab_sx1276_write_reg(radio, SX1276_REG_OP_MODE, SX1276_LONG_RANGE_MODE | mode);
}

static bool hab_sx1276_apply_config(hab_sx1276_t *radio)
{
    uint8_t modem_config1 = (uint8_t)((radio->config.bandwidth << 4) | (radio->config.coding_rate << 1));
    uint8_t modem_config2 = (uint8_t)((radio->config.spreading_factor << 4) |
                                      (radio->config.crc_enabled ? 0x04U : 0x00U));
    uint8_t tx_power = 0U;

    if (!hab_sx1276_set_op_mode(radio, SX1276_MODE_SLEEP)) {
        return false;
    }

    if (radio->hal.delay_ms != NULL) {
        radio->hal.delay_ms(radio->hal.context, 2u);
    }

    if (!hab_sx1276_set_op_mode(radio, SX1276_MODE_STDBY)) {
        return false;
    }

    if (!hab_sx1276_set_frequency(radio, radio->config.frequency_hz)) {
        return false;
    }

    if (!hab_sx1276_write_reg(radio, SX1276_REG_FIFO_TX_BASE_ADDR, 0x00U) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_LNA, 0x23U) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_OCP, 0x2BU) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_MODEM_CONFIG1, modem_config1) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_MODEM_CONFIG2, modem_config2) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_MODEM_CONFIG3, 0x04U) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_PREAMBLE_MSB, (uint8_t)(radio->config.preamble_symbols >> 8)) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_PREAMBLE_LSB, (uint8_t)radio->config.preamble_symbols) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_DIO_MAPPING1, 0x40U)) {
        return false;
    }

    if (radio->config.tx_power_dbm < 2) {
        tx_power = 0U;
    } else if (radio->config.tx_power_dbm > 17) {
        tx_power = 15U;
    } else {
        tx_power = (uint8_t)(radio->config.tx_power_dbm - 2);
    }

    return hab_sx1276_write_reg(radio, SX1276_REG_PA_CONFIG, SX1276_PA_BOOST | tx_power) &&
           hab_sx1276_write_reg(radio, SX1276_REG_PA_DAC, 0x84U) &&
           hab_sx1276_write_reg(radio, SX1276_REG_IRQ_FLAGS, 0xFFU);
}

bool hab_sx1276_init(hab_sx1276_t *radio, const hab_sx1276_hal_t *hal, const hab_sx1276_config_t *config)
{
    uint8_t version = 0u;

    if (radio == NULL || hal == NULL || config == NULL) {
        return false;
    }

    radio->hal = *hal;
    radio->config = *config;
    radio->initialized = false;

    if (radio->hal.set_reset != NULL && radio->hal.delay_ms != NULL) {
        radio->hal.set_reset(radio->hal.context, true);
        radio->hal.delay_ms(radio->hal.context, 1u);
        radio->hal.set_reset(radio->hal.context, false);
        radio->hal.delay_ms(radio->hal.context, 10u);
    }

    if (!hab_sx1276_read_reg(radio, SX1276_REG_VERSION, &version) || version == 0x00U || version == 0xFFU) {
        return false;
    }

    if (!hab_sx1276_apply_config(radio)) {
        return false;
    }

    radio->initialized = true;
    return true;
}

bool hab_sx1276_send(hab_sx1276_t *radio, const uint8_t *payload, size_t length, uint32_t timeout_ms)
{
    uint32_t elapsed_ms = 0u;
    uint8_t irq_flags = 0u;

    if (radio == NULL || payload == NULL || length == 0u || length > 255u || !radio->initialized) {
        return false;
    }

    if (!hab_sx1276_set_op_mode(radio, SX1276_MODE_STDBY) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_IRQ_FLAGS, 0xFFU) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_FIFO_ADDR_PTR, 0x00U) ||
        !hab_sx1276_write_burst(radio, SX1276_REG_FIFO, payload, length) ||
        !hab_sx1276_write_reg(radio, SX1276_REG_PAYLOAD_LENGTH, (uint8_t)length) ||
        !hab_sx1276_set_op_mode(radio, SX1276_MODE_TX)) {
        return false;
    }

    while (elapsed_ms < timeout_ms) {
        if (radio->hal.read_irq != NULL && radio->hal.read_irq(radio->hal.context)) {
            break;
        }

        if (!hab_sx1276_read_reg(radio, SX1276_REG_IRQ_FLAGS, &irq_flags)) {
            return false;
        }

        if ((irq_flags & SX1276_IRQ_TX_DONE) != 0u) {
            break;
        }

        if (radio->hal.delay_ms == NULL) {
            return false;
        }

        radio->hal.delay_ms(radio->hal.context, 1u);
        elapsed_ms++;
    }

    if (elapsed_ms >= timeout_ms) {
        return false;
    }

    return hab_sx1276_write_reg(radio, SX1276_REG_IRQ_FLAGS, 0xFFU) &&
           hab_sx1276_set_op_mode(radio, SX1276_MODE_STDBY);
}

bool hab_sx1276_sleep(hab_sx1276_t *radio)
{
    if (radio == NULL || !radio->initialized) {
        return false;
    }

    return hab_sx1276_set_op_mode(radio, SX1276_MODE_SLEEP);
}
