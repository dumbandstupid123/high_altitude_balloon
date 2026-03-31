#include "ff.h"
#include "diskio.h"

#include "poc_platform.h"

#define CMD0  (0U)
#define CMD1  (1U)
#define CMD8  (8U)
#define CMD9  (9U)
#define CMD12 (12U)
#define CMD16 (16U)
#define CMD17 (17U)
#define CMD24 (24U)
#define CMD55 (55U)
#define CMD58 (58U)
#define ACMD41 (0x80U + 41U)

#define CT_MMC   0x01U
#define CT_SD1   0x02U
#define CT_SD2   0x04U
#define CT_BLOCK 0x08U

static volatile DSTATUS g_disk_status = STA_NOINIT;
static BYTE g_card_type = 0u;

static BYTE poc_sd_xchg_spi(BYTE value)
{
    return poc_spi_transfer_byte(value);
}

static void poc_sd_rcvr_spi_multi(BYTE *buffer, UINT count)
{
    while (count-- > 0u) {
        *buffer++ = poc_sd_xchg_spi(0xFFu);
    }
}

static void poc_sd_xmit_spi_multi(const BYTE *buffer, UINT count)
{
    while (count-- > 0u) {
        (void)poc_sd_xchg_spi(*buffer++);
    }
}

static int poc_sd_wait_ready(UINT wait_ms)
{
    BYTE response = 0u;

    do {
        response = poc_sd_xchg_spi(0xFFu);
        if (response == 0xFFu) {
            return 1;
        }
        poc_delay_ms(1u);
    } while (wait_ms-- > 0u);

    return 0;
}

static void poc_sd_deselect(void)
{
    poc_sd_set_cs(false);
    (void)poc_sd_xchg_spi(0xFFu);
}

static int poc_sd_select_card(void)
{
    poc_sd_set_cs(true);
    (void)poc_sd_xchg_spi(0xFFu);
    if (poc_sd_wait_ready(500u)) {
        return 1;
    }

    poc_sd_deselect();
    return 0;
}

static int poc_sd_rcvr_datablock(BYTE *buffer, UINT bytes_to_read)
{
    BYTE token = 0u;
    UINT timeout = 200u;

    do {
        token = poc_sd_xchg_spi(0xFFu);
        poc_delay_ms(1u);
    } while (token == 0xFFu && timeout-- > 0u);

    if (token != 0xFEu) {
        return 0;
    }

    poc_sd_rcvr_spi_multi(buffer, bytes_to_read);
    (void)poc_sd_xchg_spi(0xFFu);
    (void)poc_sd_xchg_spi(0xFFu);
    return 1;
}

static int poc_sd_xmit_datablock(const BYTE *buffer, BYTE token)
{
    BYTE response = 0u;

    if (!poc_sd_wait_ready(500u)) {
        return 0;
    }

    (void)poc_sd_xchg_spi(token);

    if (token == 0xFDu) {
        return 1;
    }

    poc_sd_xmit_spi_multi(buffer, 512u);
    (void)poc_sd_xchg_spi(0xFFu);
    (void)poc_sd_xchg_spi(0xFFu);

    response = poc_sd_xchg_spi(0xFFu);
    return (response & 0x1Fu) == 0x05u;
}

static BYTE poc_sd_send_cmd(BYTE cmd, DWORD arg)
{
    BYTE command_packet[6];
    BYTE response = 0xFFu;
    UINT retries = 10u;

    if (cmd & 0x80u) {
        cmd &= 0x7Fu;
        response = poc_sd_send_cmd(CMD55, 0u);
        if (response > 1u) {
            return response;
        }
    }

    poc_sd_deselect();
    if (!poc_sd_select_card()) {
        return 0xFFu;
    }

    command_packet[0] = (BYTE)(0x40u | cmd);
    command_packet[1] = (BYTE)(arg >> 24);
    command_packet[2] = (BYTE)(arg >> 16);
    command_packet[3] = (BYTE)(arg >> 8);
    command_packet[4] = (BYTE)arg;
    command_packet[5] = 0x01u;

    if (cmd == CMD0) {
        command_packet[5] = 0x95u;
    } else if (cmd == CMD8) {
        command_packet[5] = 0x87u;
    }

    poc_sd_xmit_spi_multi(command_packet, 6u);

    if (cmd == CMD12) {
        (void)poc_sd_xchg_spi(0xFFu);
    }

    do {
        response = poc_sd_xchg_spi(0xFFu);
    } while ((response & 0x80u) != 0u && retries-- > 0u);

    return response;
}

static void poc_sd_power_on(void)
{
    UINT index = 0u;

    poc_sd_deselect();
    for (index = 0u; index < 10u; ++index) {
        (void)poc_sd_xchg_spi(0xFFu);
    }
}

DSTATUS disk_initialize(BYTE pdrv)
{
    BYTE ocr[4] = {0};
    UINT timeout = 0u;

    if (pdrv != 0u) {
        return STA_NOINIT;
    }

    poc_sd_power_on();
    g_card_type = 0u;

    if (poc_sd_send_cmd(CMD0, 0u) == 1u) {
        if (poc_sd_send_cmd(CMD8, 0x1AAu) == 1u) {
            poc_sd_rcvr_spi_multi(ocr, 4u);
            if (ocr[2] == 0x01u && ocr[3] == 0xAAu) {
                timeout = 1000u;
                while (timeout-- > 0u && poc_sd_send_cmd(ACMD41, 1UL << 30) != 0u) {
                    poc_delay_ms(1u);
                }

                if (timeout > 0u && poc_sd_send_cmd(CMD58, 0u) == 0u) {
                    poc_sd_rcvr_spi_multi(ocr, 4u);
                    g_card_type = ((ocr[0] & 0x40u) != 0u) ? (CT_SD2 | CT_BLOCK) : CT_SD2;
                }
            }
        } else {
            if (poc_sd_send_cmd(ACMD41, 0u) <= 1u) {
                g_card_type = CT_SD1;
                timeout = 1000u;
                while (timeout-- > 0u && poc_sd_send_cmd(ACMD41, 0u) != 0u) {
                    poc_delay_ms(1u);
                }
            } else {
                g_card_type = CT_MMC;
                timeout = 1000u;
                while (timeout-- > 0u && poc_sd_send_cmd(CMD1, 0u) != 0u) {
                    poc_delay_ms(1u);
                }
            }

            if (timeout == 0u || poc_sd_send_cmd(CMD16, 512u) != 0u) {
                g_card_type = 0u;
            }
        }
    }

    poc_sd_deselect();

    if (g_card_type != 0u) {
        g_disk_status &= (BYTE)~STA_NOINIT;
    } else {
        g_disk_status = STA_NOINIT;
    }

    return g_disk_status;
}

DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv != 0u) {
        return STA_NOINIT;
    }

    return g_disk_status;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != 0u || count == 0u) {
        return RES_PARERR;
    }

    if ((g_disk_status & STA_NOINIT) != 0u) {
        return RES_NOTRDY;
    }

    if ((g_card_type & CT_BLOCK) == 0u) {
        sector *= 512u;
    }

    while (count-- > 0u) {
        if (poc_sd_send_cmd(CMD17, (DWORD)sector) != 0u || !poc_sd_rcvr_datablock(buff, 512u)) {
            poc_sd_deselect();
            return RES_ERROR;
        }

        poc_sd_deselect();
        buff += 512u;
        sector++;
    }

    return RES_OK;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv != 0u || count == 0u) {
        return RES_PARERR;
    }

    if ((g_disk_status & STA_NOINIT) != 0u) {
        return RES_NOTRDY;
    }

    if ((g_disk_status & STA_PROTECT) != 0u) {
        return RES_WRPRT;
    }

    if ((g_card_type & CT_BLOCK) == 0u) {
        sector *= 512u;
    }

    while (count-- > 0u) {
        if (poc_sd_send_cmd(CMD24, (DWORD)sector) != 0u || !poc_sd_xmit_datablock(buff, 0xFEu)) {
            poc_sd_deselect();
            return RES_ERROR;
        }

        poc_sd_deselect();
        buff += 512u;
        sector++;
    }

    return RES_OK;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    BYTE csd[16] = {0};
    DWORD sectors = 0u;

    if (pdrv != 0u) {
        return RES_PARERR;
    }

    if ((g_disk_status & STA_NOINIT) != 0u) {
        return RES_NOTRDY;
    }

    switch (cmd) {
        case CTRL_SYNC:
            return poc_sd_select_card() ? (poc_sd_deselect(), RES_OK) : RES_ERROR;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = 512u;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1u;
            return RES_OK;

        case GET_SECTOR_COUNT:
            if (poc_sd_send_cmd(CMD9, 0u) != 0u || !poc_sd_rcvr_datablock(csd, 16u)) {
                poc_sd_deselect();
                return RES_ERROR;
            }

            poc_sd_deselect();

            if ((csd[0] >> 6) == 1u) {
                sectors = ((DWORD)csd[9] | ((DWORD)csd[8] << 8) | ((DWORD)(csd[7] & 0x3Fu) << 16) | 1UL) << 10;
            } else {
                BYTE read_bl_len = (BYTE)(csd[5] & 0x0Fu);
                WORD c_size = (WORD)(((WORD)(csd[8] >> 6)) | ((WORD)csd[7] << 2) | ((WORD)(csd[6] & 0x03u) << 10));
                BYTE c_size_mult = (BYTE)(((csd[10] & 0x80u) >> 7) | ((csd[9] & 0x03u) << 1));
                DWORD blocknr = (DWORD)(c_size + 1u) << (c_size_mult + 2u);
                DWORD blocklen = (DWORD)1u << read_bl_len;
                sectors = (blocknr * blocklen) / 512u;
            }

            *(LBA_t *)buff = (LBA_t)sectors;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}

DWORD get_fattime(void)
{
    return ((DWORD)(2026 - 1980) << 25) | ((DWORD)3 << 21) | ((DWORD)31 << 16);
}
