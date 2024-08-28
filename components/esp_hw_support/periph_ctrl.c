/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/clk_gate_ll.h"
#include "esp_attr.h"
#include "esp_private/periph_ctrl.h"
#include "soc/soc_caps.h"

#if SOC_MODEM_CLOCK_IS_INDEPENDENT
#include "esp_private/esp_modem_clock.h"
#endif

#ifdef __NuttX__
#include <nuttx/mutex.h>
#include <nuttx/spinlock.h>
#include <nuttx/irq.h>
#include <nuttx/queue.h>
#else
#include "freertos/FreeRTOS.h"
#endif

#ifdef __NuttX__
#define NR_IRQSTATE_FLAGS   3

#define ENTER_CRITICAL_SECTION(lock) do { \
        if (!g_periph_ctrl_lock_initialized) { \
            sq_init(&g_periph_ctrl_int_flags_free); \
            sq_init(&g_periph_ctrl_int_flags_used); \
            for (int i = 0; i < NR_IRQSTATE_FLAGS; i++) { \
                sq_addlast((sq_entry_t *)&g_periph_ctrl_int_flags[i], &g_periph_ctrl_int_flags_free); \
            } \
            g_periph_ctrl_lock_initialized = true; \
        } \
        struct irqstate_list_s *irqstate; \
        irqstate = (struct irqstate_list_s *)sq_remlast(&g_periph_ctrl_int_flags_free); \
        assert(irqstate != NULL); \
        irqstate->flags = spin_lock_irqsave(lock); \
        sq_addlast((sq_entry_t *)irqstate, &g_periph_ctrl_int_flags_used); \
    } while(0)

#define LEAVE_CRITICAL_SECTION(lock) do { \
        struct irqstate_list_s *irqstate; \
        irqstate = (struct irqstate_list_s *)sq_remlast(&g_periph_ctrl_int_flags_used); \
        assert(irqstate != NULL); \
        spin_unlock_irqrestore((lock), irqstate->flags); \
        sq_addlast((sq_entry_t *)irqstate, &g_periph_ctrl_int_flags_free); \
    } while(0)

struct irqstate_list_s
{
  struct irqstate_list_s *flink;
  irqstate_t flags;
};

static spinlock_t periph_spinlock;
static bool g_periph_ctrl_lock_initialized = false;
static sq_queue_t g_periph_ctrl_int_flags_free;
static sq_queue_t g_periph_ctrl_int_flags_used;
static struct irqstate_list_s g_periph_ctrl_int_flags[NR_IRQSTATE_FLAGS];
static spinlock_t periph_spinlock;
#else
#define ENTER_CRITICAL_SECTION(lock)    portENTER_CRITICAL_SAFE(lock)
#define LEAVE_CRITICAL_SECTION(lock)    portEXIT_CRITICAL_SAFE(lock)

static portMUX_TYPE periph_spinlock = portMUX_INITIALIZER_UNLOCKED;
#endif

static uint8_t ref_counts[PERIPH_MODULE_MAX] = {0};

void periph_module_enable(periph_module_t periph)
{
    assert(periph < PERIPH_MODULE_MAX);
    ENTER_CRITICAL_SECTION(&periph_spinlock);
    if (ref_counts[periph] == 0) {
        periph_ll_enable_clk_clear_rst(periph);
    }
    ref_counts[periph]++;
    LEAVE_CRITICAL_SECTION(&periph_spinlock);
}

void periph_module_disable(periph_module_t periph)
{
    assert(periph < PERIPH_MODULE_MAX);
    ENTER_CRITICAL_SECTION(&periph_spinlock);
    ref_counts[periph]--;
    if (ref_counts[periph] == 0) {
        periph_ll_disable_clk_set_rst(periph);
    }
    LEAVE_CRITICAL_SECTION(&periph_spinlock);
}

void periph_module_reset(periph_module_t periph)
{
    assert(periph < PERIPH_MODULE_MAX);
    ENTER_CRITICAL_SECTION(&periph_spinlock);
    periph_ll_reset(periph);
    LEAVE_CRITICAL_SECTION(&periph_spinlock);
}

#if !SOC_IEEE802154_BLE_ONLY
IRAM_ATTR void wifi_bt_common_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_PHY_MODULE);
#else
    ENTER_CRITICAL_SECTION(&periph_spinlock);
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_enable_clk();
    }
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]++;
    LEAVE_CRITICAL_SECTION(&periph_spinlock);
#endif
}

IRAM_ATTR void wifi_bt_common_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_PHY_MODULE);
#else
    ENTER_CRITICAL_SECTION(&periph_spinlock);
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]--;
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_disable_clk();
    }
    LEAVE_CRITICAL_SECTION(&periph_spinlock);
#endif
}
#endif

#if CONFIG_ESP_WIFI_ENABLED
void wifi_module_enable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_enable(PERIPH_WIFI_MODULE);
#else
    ENTER_CRITICAL_SECTION(&periph_spinlock);
    periph_ll_wifi_module_enable_clk_clear_rst();
    LEAVE_CRITICAL_SECTION(&periph_spinlock);
#endif
}

void wifi_module_disable(void)
{
#if SOC_MODEM_CLOCK_IS_INDEPENDENT
    modem_clock_module_disable(PERIPH_WIFI_MODULE);
#else
    ENTER_CRITICAL_SECTION(&periph_spinlock);
    periph_ll_wifi_module_disable_clk_set_rst();
    LEAVE_CRITICAL_SECTION(&periph_spinlock);
#endif
}
#endif // CONFIG_ESP_WIFI_ENABLED
