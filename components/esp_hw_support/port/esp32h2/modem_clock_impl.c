/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_attr.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_modem_clock.h"
#include "esp_private/regi2c_ctrl.h"
#if CONFIG_IDF_TARGET_ESP32H2
#include "soc/rtc.h"
#endif // CONFIG_IDF_TARGET_ESP32H2

/* Clock dependency definitions */
#define BLE_CLOCK_DEPS          (BIT(MODEM_CLOCK_BLE_MAC) | BIT(MODEM_CLOCK_BT_I154_COMMON_BB) | BIT(MODEM_CLOCK_ETM) | BIT(MODEM_CLOCK_COEXIST))
#define IEEE802154_CLOCK_DEPS   (BIT(MODEM_CLOCK_802154_MAC) | BIT(MODEM_CLOCK_BT_I154_COMMON_BB) | BIT(MODEM_CLOCK_ETM) | BIT(MODEM_CLOCK_COEXIST))
#define COEXIST_CLOCK_DEPS      (BIT(MODEM_CLOCK_COEXIST))
#define I2C_ANA_MST_CLOCK_DEPS  (BIT(MODEM_CLOCK_I2C_MASTER))
#define PHY_CLOCK_DEPS          (I2C_ANA_MST_CLOCK_DEPS | BIT(MODEM_CLOCK_MODEM_ADC_COMMON_FE) | BIT(MODEM_CLOCK_MODEM_PRIVATE_FE))
#define MODEM_ETM_CLOCK_DEPS    (BIT(MODEM_CLOCK_ETM))
#define MODEM_ADC_COMMON_FE_CLOCK_DEPS  (BIT(MODEM_CLOCK_MODEM_ADC_COMMON_FE))
#define PHY_CALIBRATION_BT_I154_CLOCK_DEPS (BIT(MODEM_CLOCK_BT_I154_COMMON_BB))
#define PHY_CALIBRATION_WIFI_CLOCK_DEPS     0
#define PHY_CALIBRATION_CLOCK_DEPS          (PHY_CALIBRATION_WIFI_CLOCK_DEPS | PHY_CALIBRATION_BT_I154_CLOCK_DEPS)

uint32_t modem_clock_get_module_deps(shared_periph_module_t module)
{
    uint32_t deps = 0;
    switch (module) {
        case PERIPH_ANA_I2C_MASTER_MODULE:      deps = I2C_ANA_MST_CLOCK_DEPS;          break;
        case PERIPH_PHY_MODULE:                 deps = PHY_CLOCK_DEPS;                  break;
        case PERIPH_MODEM_ADC_COMMON_FE_MODULE: deps = MODEM_ADC_COMMON_FE_CLOCK_DEPS;  break;
        case PERIPH_COEX_MODULE:                deps = COEXIST_CLOCK_DEPS;              break;
        case PERIPH_BT_MODULE:                  deps = BLE_CLOCK_DEPS;                  break;
        case PERIPH_PHY_CALIBRATION_MODULE:     deps = PHY_CALIBRATION_CLOCK_DEPS;      break;
        case PERIPH_IEEE802154_MODULE:          deps = IEEE802154_CLOCK_DEPS;           break;
        case PERIPH_MODEM_ETM_MODULE:           deps = MODEM_ETM_CLOCK_DEPS;            break;
        default:
            assert(0);
    }
    return deps;
}

static void IRAM_ATTR modem_clock_ble_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_bt_mac_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_modem_sec_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_ble_timer_clock(ctx->hal->syscon_dev, enable);
}

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_ble_mac_check_enable(modem_clock_context_t *ctx)
{
    bool all_clock_enabled = true;
    all_clock_enabled &= modem_syscon_ll_bt_mac_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_modem_sec_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_ble_timer_clock_is_enabled(ctx->hal->syscon_dev);
    return all_clock_enabled ? ESP_OK : ESP_FAIL;
}
#endif

static void IRAM_ATTR modem_clock_ble_i154_bb_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_bt_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_bt_clock(ctx->hal->syscon_dev, enable);
}

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_ble_i154_bb_check_enable(modem_clock_context_t *ctx)
{
    bool all_clock_enabled = true;
    all_clock_enabled &= modem_syscon_ll_bt_apb_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_bt_clock_is_enabled(ctx->hal->syscon_dev);
    return all_clock_enabled ? ESP_OK : ESP_FAIL;
}
#endif

static void IRAM_ATTR modem_clock_ieee802154_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_ieee802154_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_ieee802154_mac_clock(ctx->hal->syscon_dev, enable);
}

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_ieee802154_mac_check_enable(modem_clock_context_t *ctx)
{
    bool all_clock_enabled = true;
    all_clock_enabled &= modem_syscon_ll_ieee802154_apb_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_ieee802154_mac_clock_is_enabled(ctx->hal->syscon_dev);
    return all_clock_enabled ? ESP_OK : ESP_FAIL;
}
#endif

static void IRAM_ATTR modem_clock_coex_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_lpcon_ll_enable_coex_clock(ctx->hal->lpcon_dev, enable);
}

static void IRAM_ATTR modem_clock_modem_adc_common_fe_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_clock_hal_enable_modem_common_fe_clock(ctx->hal, enable);
}

static void IRAM_ATTR modem_clock_modem_private_fe_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_clock_hal_enable_modem_private_fe_clock(ctx->hal, enable);
}

static void IRAM_ATTR modem_clock_i2c_master_configure(modem_clock_context_t *ctx, bool enable)
{
    if (enable) {
        ANALOG_CLOCK_ENABLE();
    } else {
        ANALOG_CLOCK_DISABLE();
    }
}

static void IRAM_ATTR modem_clock_etm_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_etm_clock(ctx->hal->syscon_dev, enable);
}

static void IRAM_ATTR modem_clock_data_dump_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_data_dump_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_data_dump_mux_clock(ctx->hal->syscon_dev, enable);
}

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_coex_check_enable(modem_clock_context_t *ctx)
{
    return modem_lpcon_ll_coex_clock_is_enabled(ctx->hal->lpcon_dev) ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_modem_adc_common_fe_check_enable(modem_clock_context_t *ctx)
{
    return modem_clock_hal_modem_common_fe_clock_is_enabled(ctx->hal) ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_modem_private_fe_check_enable(modem_clock_context_t *ctx)
{
    return modem_clock_hal_modem_private_fe_clock_is_enabled(ctx->hal) ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_i2c_master_check_enable(modem_clock_context_t *ctx)
{
    return ANALOG_CLOCK_IS_ENABLED() ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_etm_check_enable(modem_clock_context_t *ctx)
{
    return modem_syscon_ll_etm_clock_is_enabled(ctx->hal->syscon_dev) ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_data_dump_check_enable(modem_clock_context_t *ctx)
{
    bool all_clock_enabled = true;
    all_clock_enabled &= modem_syscon_ll_data_dump_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_data_dump_mux_clock_is_enabled(ctx->hal->syscon_dev);
    return all_clock_enabled ? ESP_OK : ESP_FAIL;
}
#endif

DRAM_ATTR modem_clock_device_context_t g_modem_clock_dev[MODEM_CLOCK_DEVICE_MAX] = {
    [MODEM_CLOCK_MODEM_ADC_COMMON_FE]   = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_modem_adc_common_fe_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_modem_adc_common_fe_check_enable
#endif
    },
    [MODEM_CLOCK_MODEM_PRIVATE_FE]      = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_modem_private_fe_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_modem_private_fe_check_enable
#endif
    },
    [MODEM_CLOCK_COEXIST]               = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_coex_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_coex_check_enable
#endif
    },
// ANALOG_CLOCK_ENABLE/DISABLE has its own ref_cnt management.
    [MODEM_CLOCK_I2C_MASTER]            = { .refs = 0, .with_refcnt = false,    .configure = modem_clock_i2c_master_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_i2c_master_check_enable
#endif
    },
    [MODEM_CLOCK_ETM]                   = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_etm_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_etm_check_enable
#endif
    },
    [MODEM_CLOCK_BLE_MAC]               = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_ble_mac_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_ble_mac_check_enable
#endif
    },
    [MODEM_CLOCK_BT_I154_COMMON_BB]     = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_ble_i154_bb_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_ble_i154_bb_check_enable
#endif
    },
    [MODEM_CLOCK_802154_MAC]            = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_ieee802154_mac_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_ieee802154_mac_check_enable
#endif
    },
    [MODEM_CLOCK_DATADUMP]              = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_data_dump_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_data_dump_check_enable
#endif
    }
};

#if CONFIG_IDF_TARGET_ESP32H2
/* rc32k clock enable status before selecting BLE RTC timer clock source */
static bool rc_clk_en = true;
void modem_clock_select_ble_rtc_timer_clk_workaround(modem_clock_context_t *ctx, bool enable, modem_clock_lpclk_src_t src)
{
    if (enable) {
        bool selected = (src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) ||
                        (src == MODEM_CLOCK_LPCLK_SRC_RC_SLOW);
        if (selected) {
            rc_clk_en = clk_ll_rc32k_is_enabled();
            if (!rc_clk_en) {
                rtc_clk_rc32k_enable(true);
            }
            modem_clock_hal_select_ble_rtc_timer_lpclk_source(ctx->hal, MODEM_CLOCK_LPCLK_SRC_RC32K);
        }
    } else {
        if (!rc_clk_en) {
            extern void r_esp_ble_rtc_ticks_delay(uint32_t ticks);
            r_esp_ble_rtc_ticks_delay(2);
            rtc_clk_rc32k_enable(false);
        }
    }
}
#endif // CONFIG_IDF_TARGET_ESP32H2
