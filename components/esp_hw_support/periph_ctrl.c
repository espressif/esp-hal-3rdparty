/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/clk_gate_ll.h"
#include "esp_attr.h"
#include "esp_private/periph_ctrl.h"

#if SOC_MODEM_CLOCK_IS_INDEPENDENT
#include "esp_private/esp_modem_clock.h"
#endif

#ifdef __ZEPHYR__
#include <zephyr/kernel.h>
#elif defined(__NuttX__)
#include <nuttx/irq.h>
#else
#include "freertos/FreeRTOS.h"
#endif

#ifdef __ZEPHYR__
#define ENTER_CRITICAL_SECTION(state)   do { (state) = irq_lock(); } while(0)
#define LEAVE_CRITICAL_SECTION(state)   irq_unlock((state))

static unsigned int periph_spinlock;
#elif defined(__NuttX__)
#define ENTER_CRITICAL_SECTION(state)   do { (state) = enter_critical_section(); } while(0)
#define LEAVE_CRITICAL_SECTION(state)   leave_critical_section((state))

static irqstate_t periph_spinlock;
#else
#define ENTER_CRITICAL_SECTION(state)   portENTER_CRITICAL_SAFE(&(state))
#define LEAVE_CRITICAL_SECTION(state)   portEXIT_CRITICAL_SAFE(&(state))

static portMUX_TYPE periph_spinlock = portMUX_INITIALIZER_UNLOCKED;
#endif

static uint8_t ref_counts[PERIPH_MODULE_MAX] = {0};

void periph_module_enable(periph_module_t periph)
{
    assert(periph < PERIPH_MODULE_MAX);
    ENTER_CRITICAL_SECTION(periph_spinlock);
    if (ref_counts[periph] == 0) {
        periph_ll_enable_clk_clear_rst(periph);
    }
    ref_counts[periph]++;
    LEAVE_CRITICAL_SECTION(periph_spinlock);
}

void periph_module_disable(periph_module_t periph)
{
    assert(periph < PERIPH_MODULE_MAX);
    ENTER_CRITICAL_SECTION(periph_spinlock);
    ref_counts[periph]--;
    if (ref_counts[periph] == 0) {
        periph_ll_disable_clk_set_rst(periph);
    }
    LEAVE_CRITICAL_SECTION(periph_spinlock);
}

void periph_module_reset(periph_module_t periph)
{
    assert(periph < PERIPH_MODULE_MAX);
    ENTER_CRITICAL_SECTION(periph_spinlock);
    periph_ll_reset(periph);
    LEAVE_CRITICAL_SECTION(periph_spinlock);
}

#if !SOC_IEEE802154_BLE_ONLY
IRAM_ATTR void wifi_bt_common_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_PHY_MODULE);
#else
    ENTER_CRITICAL_SECTION(periph_spinlock);
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]++;
    LEAVE_CRITICAL_SECTION(periph_spinlock);
#endif
}

IRAM_ATTR void wifi_bt_common_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_PHY_MODULE);
#else
    ENTER_CRITICAL_SECTION(periph_spinlock);
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]--;
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_disable_clk_set_rst();
    }
    LEAVE_CRITICAL_SECTION(periph_spinlock);
#endif
}
#endif

#if CONFIG_ESP_WIFI_ENABLED
void wifi_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_WIFI_MODULE);
#else
    ENTER_CRITICAL_SECTION(periph_spinlock);
    periph_ll_wifi_module_enable_clk_clear_rst();
    LEAVE_CRITICAL_SECTION(periph_spinlock);
#endif
}

void wifi_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_WIFI_MODULE);
#else
    ENTER_CRITICAL_SECTION(periph_spinlock);
    periph_ll_wifi_module_disable_clk_set_rst();
    LEAVE_CRITICAL_SECTION(periph_spinlock);
#endif
}
#endif // CONFIG_ESP_WIFI_ENABLED
