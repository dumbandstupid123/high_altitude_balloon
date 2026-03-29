/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "include/hab_app.h"
#include "include/hab_protocol.h"
#include "include/platform/hab_mspm0_ccs.h"
#include "ti_msp_dl_config.h"

static hab_app_t g_hab_app;
static hab_platform_t g_hab_platform;
static volatile bool g_scheduler_tick_pending = false;
static volatile bool g_gps_line_ready = false;
static volatile bool g_gps_line_truncated = false;
static volatile bool g_gps_capture_active = false;
static volatile size_t g_gps_line_len = 0u;
static char g_gps_line_buffer[96];
static hab_state_t g_last_reported_state = HAB_STATE_BOOT;
static uint32_t g_last_reported_fault_flags = 0u;

#define HAB_GPS_SOFT_UART_BAUD 9600U
#define HAB_GPS_SOFT_UART_BIT_US (1000000U / HAB_GPS_SOFT_UART_BAUD)
#define HAB_GPS_SOFT_UART_START_SAMPLE_US (HAB_GPS_SOFT_UART_BIT_US + (HAB_GPS_SOFT_UART_BIT_US / 2U))

static void hab_debug_uart_write_char(char ch)
{
    DL_UART_Main_transmitDataBlocking(UART_DEBUG_INST, (uint8_t)ch);
}

static void hab_debug_uart_write_string(const char *text)
{
    size_t index = 0u;

    if (text == NULL) {
        return;
    }

    while (text[index] != '\0') {
        hab_debug_uart_write_char(text[index]);
        index++;
    }
}

static void hab_debug_uart_write_line(const char *text)
{
    hab_debug_uart_write_string(text);
    hab_debug_uart_write_string("\r\n");
}

static void hab_soft_uart_delay_us(uint32_t delay_us)
{
    if (delay_us == 0u) {
        return;
    }

    delay_cycles((CPUCLK_FREQ / 1000000u) * delay_us);
}

static bool hab_soft_uart_read_gps_byte(char *byte_out)
{
    uint8_t bit = 0u;
    uint8_t value = 0u;

    if (byte_out == NULL) {
        return false;
    }

    hab_soft_uart_delay_us(HAB_GPS_SOFT_UART_START_SAMPLE_US);

    for (bit = 0u; bit < 8u; ++bit) {
        if (DL_GPIO_readPins(GPIO_AUX_GPS_RX_SOFT_PORT, GPIO_AUX_GPS_RX_SOFT_PIN) != 0u) {
            value |= (uint8_t)(1u << bit);
        }
        hab_soft_uart_delay_us(HAB_GPS_SOFT_UART_BIT_US);
    }

    if (DL_GPIO_readPins(GPIO_AUX_GPS_RX_SOFT_PORT, GPIO_AUX_GPS_RX_SOFT_PIN) == 0u) {
        return false;
    }

    *byte_out = (char)value;
    return true;
}

static void hab_capture_gps_debug_byte(char gps_byte)
{
    if (g_gps_line_ready) {
        return;
    }

    if (gps_byte == '$') {
        g_gps_line_buffer[0] = '$';
        g_gps_line_len = 1u;
        g_gps_line_truncated = false;
        g_gps_capture_active = true;
        return;
    }

    if (!g_gps_capture_active) {
        return;
    }

    if (gps_byte == '\r') {
        return;
    }

    if (gps_byte == '\n') {
        if (g_gps_line_len >= sizeof(g_gps_line_buffer)) {
            g_gps_line_len = sizeof(g_gps_line_buffer) - 1u;
        }
        g_gps_line_buffer[g_gps_line_len] = '\0';
        g_gps_line_ready = true;
        g_gps_capture_active = false;
        return;
    }

    if (g_gps_line_len + 1u < sizeof(g_gps_line_buffer)) {
        g_gps_line_buffer[g_gps_line_len++] = gps_byte;
    } else {
        g_gps_line_truncated = true;
    }
}

static void hab_flush_debug_gps_line(void)
{
    char line_copy[sizeof(g_gps_line_buffer)];
    bool line_ready = false;
    bool line_truncated = false;

    NVIC_DisableIRQ(GPIO_AUX_INT_IRQN);
    if (g_gps_line_ready) {
        memcpy(line_copy, g_gps_line_buffer, sizeof(line_copy));
        g_gps_line_ready = false;
        line_truncated = g_gps_line_truncated;
        g_gps_line_truncated = false;
        line_ready = true;
    }
    NVIC_EnableIRQ(GPIO_AUX_INT_IRQN);

    if (!line_ready) {
        return;
    }

    hab_debug_uart_write_string("GPS ");
    hab_debug_uart_write_string(line_copy);
    if (line_truncated) {
        hab_debug_uart_write_string(" [truncated]");
    }
    hab_debug_uart_write_string("\r\n");
}

static void hab_debug_uart_write_status(void)
{
    char line[192];
    const hab_runtime_t *runtime = hab_app_runtime(&g_hab_app);

    if (runtime == NULL) {
        return;
    }

    (void)snprintf(line,
                   sizeof(line),
                   "STAT s=%lu state=%s gps_fix=%u sats=%u lat=%.6f lon=%.6f gps_alt=%.1f p_alt=%.1f temp=%.1f hum=%.1f fault=0x%02lX",
                   (unsigned long)runtime->boot_seconds,
                   hab_state_name(runtime->state),
                   runtime->gps.valid_fix ? 1u : 0u,
                   (unsigned int)runtime->gps.satellites,
                   runtime->gps.latitude_deg,
                   runtime->gps.longitude_deg,
                   runtime->gps.altitude_m,
                   runtime->env.filtered_altitude_m,
                   runtime->env.temperature_c,
                   runtime->env.humidity_percent,
                   (unsigned long)runtime->fault_flags);
    hab_debug_uart_write_line(line);
}

static void hab_debug_uart_report_runtime_changes(void)
{
    char line[128];
    const hab_runtime_t *runtime = hab_app_runtime(&g_hab_app);

    if (runtime == NULL) {
        return;
    }

    if (runtime->state != g_last_reported_state) {
        (void)snprintf(line,
                       sizeof(line),
                       "EVT state %s -> %s",
                       hab_state_name(g_last_reported_state),
                       hab_state_name(runtime->state));
        hab_debug_uart_write_line(line);
        g_last_reported_state = runtime->state;
    }

    if (runtime->fault_flags != g_last_reported_fault_flags) {
        (void)snprintf(line,
                       sizeof(line),
                       "EVT faults 0x%02lX -> 0x%02lX",
                       (unsigned long)g_last_reported_fault_flags,
                       (unsigned long)runtime->fault_flags);
        hab_debug_uart_write_line(line);
        g_last_reported_fault_flags = runtime->fault_flags;
    }
}

int main(void)
{
    SYSCFG_DL_init();

    if (!hab_platform_init_mspm0_ccs(&g_hab_platform)) {
        __BKPT(0);
        while (1) {
        }
    }

    hab_app_init(&g_hab_app);

    if (!hab_app_bootstrap(&g_hab_app, &g_hab_platform)) {
        __BKPT(0);
        while (1) {
        }
    }

    NVIC_ClearPendingIRQ(GPIO_AUX_INT_IRQN);
    NVIC_EnableIRQ(GPIO_AUX_INT_IRQN);
    NVIC_ClearPendingIRQ(TIMER_SCHED_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_SCHED_INST_INT_IRQN);
    DL_SYSCTL_disableSleepOnExit();
    DL_TimerG_startCounter(TIMER_SCHED_INST);
    hab_debug_uart_write_line("HAB boot");
    hab_debug_uart_write_line("GPS soft UART RX active on PA10");
    g_last_reported_state = hab_app_runtime(&g_hab_app)->state;
    g_last_reported_fault_flags = hab_app_runtime(&g_hab_app)->fault_flags;
    hab_debug_uart_write_line("EVT runtime initialized");

    while (1) {
        if (g_scheduler_tick_pending) {
            g_scheduler_tick_pending = false;
            hab_app_on_tick(&g_hab_app, &g_hab_platform);
            hab_flush_debug_gps_line();
            hab_debug_uart_report_runtime_changes();
            hab_debug_uart_write_status();
        } else if (g_gps_line_ready) {
            hab_flush_debug_gps_line();
        } else {
            __WFI();
        }
    }
}

void GROUP1_IRQHandler(void)
{
    switch (DL_GPIO_getPendingInterrupt(GPIO_AUX_GPS_RX_SOFT_PORT)) {
        case GPIO_AUX_GPS_RX_SOFT_IIDX: {
            char gps_byte = '\0';

            if (DL_GPIO_readPins(GPIO_AUX_GPS_RX_SOFT_PORT, GPIO_AUX_GPS_RX_SOFT_PIN) == 0u &&
                hab_soft_uart_read_gps_byte(&gps_byte)) {
                hab_app_on_gps_byte(&g_hab_app, gps_byte);
                hab_capture_gps_debug_byte(gps_byte);
            }
            break;
        }
        default:
            break;
    }
}

void TIMER_SCHED_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_SCHED_INST)) {
        case DL_TIMER_IIDX_ZERO:
            g_scheduler_tick_pending = true;
            break;
        default:
            break;
    }
}
