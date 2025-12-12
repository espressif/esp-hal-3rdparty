/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "hal/modem_clock_types.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/critical_section.h"

#if SOC_MODEM_CLOCK_IS_INDEPENDENT && SOC_MODEM_CLOCK_SUPPORTED
#include "hal/modem_clock_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MODEM_CLOCK_IS_INDEPENDENT && SOC_MODEM_CLOCK_SUPPORTED
// Please define the frequently called modules in the low bit,
// which will improve the execution efficiency
typedef enum {
    MODEM_CLOCK_MODEM_ADC_COMMON_FE,
    MODEM_CLOCK_MODEM_PRIVATE_FE,
    MODEM_CLOCK_COEXIST,
#if ANA_I2C_MST_CLK_HAS_ROOT_GATING
    MODEM_CLOCK_I2C_MASTER,
#endif
#if SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT
    MODEM_CLOCK_WIFI_APB,
    MODEM_CLOCK_WIFI_BB_44M,
#endif
#if SOC_WIFI_SUPPORTED
    MODEM_CLOCK_WIFI_MAC,
    MODEM_CLOCK_WIFI_BB,
#endif
    MODEM_CLOCK_ETM,
#if SOC_BT_SUPPORTED
    MODEM_CLOCK_BLE_MAC,
#endif
#if SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
    MODEM_CLOCK_BT_I154_COMMON_BB,
#endif
#if SOC_IEEE802154_SUPPORTED
    MODEM_CLOCK_802154_MAC,
#endif
    MODEM_CLOCK_DATADUMP,
    MODEM_CLOCK_DEVICE_MAX
} modem_clock_device_t;

#define MODEM_STATUS_IDLE           (0)
#define MODEM_STATUS_WIFI_INITED    (0x1UL)

struct modem_clock_context;

typedef struct {
    int16_t     refs;               /* Reference count for this device, if with_refcnt is enabled */
    uint16_t    with_refcnt : 1;    /* Enable reference count management (true=use refs, false=ignore refs) */
    uint16_t    reserved    : 15;   /* reserved for 15 bits aligned */
    void        (*configure)(struct modem_clock_context *, bool);
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
    esp_err_t   (*check_enable)(struct modem_clock_context *);
#endif
} modem_clock_device_context_t;

typedef struct modem_clock_context {
    modem_clock_hal_context_t *hal;
    spinlock_t                lock;
    modem_clock_device_context_t *dev;
#if SOC_PM_SUPPORT_PMU_MODEM_STATE
    const uint8_t *initial_gating_mode;
#endif
    /* the low-power clock source for each module */
    modem_clock_lpclk_src_t lpclk_src[PERIPH_MODEM_MODULE_NUM];
#if SOC_WIFI_SUPPORTED
    uint32_t modem_status;
#endif
} modem_clock_context_t;

/**
 * @brief Get module clock dependencies
 * Each chip implements this function in its port file
 * @param module The modem module
 * @return Bitmask of clock dependencies
 */
uint32_t modem_clock_get_module_deps(shared_periph_module_t module);

/**
 * @brief Modem clock device context array
 * Each chip defines this array in its port file
 */
extern modem_clock_device_context_t g_modem_clock_dev[MODEM_CLOCK_DEVICE_MAX];

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
/**
 * @brief Initial gating mode for each clock domain
 * Each chip defines this array in its port file
 */
extern const uint8_t g_initial_gating_mode[MODEM_CLOCK_DOMAIN_MAX];
#endif

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
/* the ICG code's bit 0, 1 and 2 indicates the ICG state
 * of pmu SLEEP, MODEM and ACTIVE mode respectively */
 #define ICG_NOGATING_ACTIVE (BIT(PMU_HP_ICG_MODEM_CODE_ACTIVE))
 #define ICG_NOGATING_SLEEP  (BIT(PMU_HP_ICG_MODEM_CODE_SLEEP))
 #define ICG_NOGATING_MODEM  (BIT(PMU_HP_ICG_MODEM_CODE_MODEM))
#endif // SOC_PM_SUPPORT_PMU_MODEM_STATE
#endif // SOC_MODEM_CLOCK_IS_INDEPENDENT && SOC_MODEM_CLOCK_SUPPORTED

/**
 * @brief Enable the clock of modem module
 *
 * Solve the clock dependency between modem modules, For example, the wifi
 * module depends on the wifi mac, wifi baseband and FE, when wifi module
 * clock is enabled, the wifi MAC, baseband and FE clocks will be enabled
 *
 * This interface and modem_clock_module_disable will jointly maintain the
 * ref_cnt of each device clock source. The ref_cnt indicates how many modules
 * are relying on the clock source. Each enable ops will add 1 to the ref_cnt of
 * the clock source that the module depends on, and only when the ref_cnt of
 * the module is from 0 to 1 will the clock enable be actually configured.
 *
 * !!! Do not use the hal/ll layer interface to configure the clock for the
 * consistency of the hardware state maintained in the driver and the hardware
 * actual state.
 *
 * @param module  modem module
 */
void modem_clock_module_enable(shared_periph_module_t module);

/**
 * @brief Disable the clock of modem module
 *
 * This interface and modem_clock_module_enable will jointly maintain the ref_cnt
 * of each device clock source. The ref_cnt indicates how many modules are relying
 * on the clock source. Each disable ops will minus 1 to the ref_cnt of the clock
 * source that the module depends on, and only when the ref_cnt of the module is
 * from 1 to 0 will the clock disable be actually configured.
 *
 * !!! Do not use the hal/ll layer interface to configure the clock for the
 * consistency of the hardware state maintained in the driver and the hardware
 * actual state.
 *
 * @param module  modem module
 */
void modem_clock_module_disable(shared_periph_module_t module);

/**
 * @brief Gets the clock bitmask associated with the specified modem module.
 *
 * This function returns the complete set of clock-enable bits that correspond
 * to @p module.
 *
 * @param module  Target shared peripheral clock module.
 *
 * @return Bitmask of clock-enable bits for the given module.
 */
uint32_t modem_clock_module_bits_get(shared_periph_module_t module);

#if SOC_WIFI_SUPPORTED
/**
 * @brief Set Wi-Fi initialization status.
 *
 * @param inited Wi-Fi initialization status.
 */
void modem_clock_configure_wifi_status(bool inited);
#endif

/**
 * @brief Reset the mac of modem module
 *
 * @param module  modem module, must be one of
 *    PERIPH_WIFI_MODULE / PERIPH_BT_MODULE /PERIPH_IEEE802154_MODULE
 */
void modem_clock_module_mac_reset(shared_periph_module_t module);

#if SOC_BLE_USE_WIFI_PWR_CLK_WORKAROUND
/**
 * @brief Apply or clear BT low-power clock workaround for enabling Wi-Fi power clock
 *
 * @param ctx    modem clock context
 * @param select true to apply (ungate); false to clear (gate)
 * @param src    lowpower clock source
 */
void modem_clock_bt_wifipwr_clk_workaround(modem_clock_context_t *ctx, bool select, modem_clock_lpclk_src_t src);
#endif

/**
 * @brief Select the modem module lowpower clock source and configure the clock divider
 *
 * @param module  modem module
 * @param src     lowpower clock source
 * @param divider divider value to lowpower clock source
 */
void modem_clock_select_lp_clock_source(shared_periph_module_t module, modem_clock_lpclk_src_t src, uint32_t divider);

/**
 * @brief Disable lowpower clock source selection
 * @param module  modem module
 */
void modem_clock_deselect_lp_clock_source(shared_periph_module_t module);

/**
* @brief Disable all modem module's lowpower clock source selection
 */
void modem_clock_deselect_all_module_lp_clock_source(void);

#if CONFIG_IDF_TARGET_ESP32H2
/**
 * @brief BLE RTC timer LPCLK workaround on ESP32H2
 *
 * @param ctx    modem clock context
 * @param enable true to enable rc32k; false to disable rc32k
 * @param src    lowpower clock source
 */
void modem_clock_select_ble_rtc_timer_clk_workaround(modem_clock_context_t *ctx, bool enable, modem_clock_lpclk_src_t src);
#endif

/**
 * @brief Reset wifi mac
 */
void modem_clock_wifi_mac_reset(void);

/**
 * @brief Enable clock registers which shared by both modem and ADC. Need a ref count to enable/disable them
 *
 * @param enable true: enable; false: disable
 */
void modem_clock_shared_enable(bool enable);

#ifdef __cplusplus
}
#endif
