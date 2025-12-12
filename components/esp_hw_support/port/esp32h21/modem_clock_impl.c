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

/* Clock dependency definitions */
#define BLE_CLOCK_DEPS          (BIT(MODEM_CLOCK_BLE_MAC) | BIT(MODEM_CLOCK_BT_I154_COMMON_BB) | BIT(MODEM_CLOCK_ETM) | BIT(MODEM_CLOCK_COEXIST))
#define IEEE802154_CLOCK_DEPS   (BIT(MODEM_CLOCK_802154_MAC) | BIT(MODEM_CLOCK_BT_I154_COMMON_BB) | BIT(MODEM_CLOCK_ETM) | BIT(MODEM_CLOCK_COEXIST))
#define COEXIST_CLOCK_DEPS      (BIT(MODEM_CLOCK_COEXIST))
#if ANA_I2C_MST_CLK_HAS_ROOT_GATING
#define I2C_ANA_MST_CLOCK_DEPS  (BIT(MODEM_CLOCK_I2C_MASTER))
#else
#define I2C_ANA_MST_CLOCK_DEPS  (0)
#endif
#define PHY_CLOCK_DEPS          (I2C_ANA_MST_CLOCK_DEPS | BIT(MODEM_CLOCK_MODEM_ADC_COMMON_FE) | BIT(MODEM_CLOCK_MODEM_PRIVATE_FE))
#define MODEM_ETM_CLOCK_DEPS    (BIT(MODEM_CLOCK_ETM))
#define MODEM_ADC_COMMON_FE_CLOCK_DEPS  (BIT(MODEM_CLOCK_MODEM_ADC_COMMON_FE))
#if SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT
#define PHY_CALIBRATION_CLOCK_DEPS      (BIT(MODEM_CLOCK_WIFI_APB) | BIT(MODEM_CLOCK_WIFI_BB_44M))
#define PHY_CALIBRATION_WIFI_CLOCK_DEPS     (BIT(MODEM_CLOCK_WIFI_APB) | BIT(MODEM_CLOCK_WIFI_BB) | BIT(MODEM_CLOCK_WIFI_BB_44M))
#define PHY_CALIBRATION_BT_I154_CLOCK_DEPS (BIT(MODEM_CLOCK_WIFI_APB) | BIT(MODEM_CLOCK_WIFI_BB_44M) | BIT(MODEM_CLOCK_BT_I154_COMMON_BB))
#else
#define PHY_CALIBRATION_WIFI_CLOCK_DEPS     (BIT(MODEM_CLOCK_WIFI_BB))
#define PHY_CALIBRATION_BT_I154_CLOCK_DEPS (BIT(MODEM_CLOCK_BT_I154_COMMON_BB))
#endif
#ifndef SOC_WIFI_SUPPORTED
#undef  PHY_CALIBRATION_WIFI_CLOCK_DEPS
#define PHY_CALIBRATION_WIFI_CLOCK_DEPS     0
#endif
#if !defined(SOC_BT_SUPPORTED) && !defined(SOC_IEEE802154_SUPPORTED)
#undef  PHY_CALIBRATION_BT_I154_CLOCK_DEPS
#define PHY_CALIBRATION_BT_I154_CLOCK_DEPS 0
#endif
#define PHY_CALIBRATION_CLOCK_DEPS          (PHY_CALIBRATION_WIFI_CLOCK_DEPS | PHY_CALIBRATION_BT_I154_CLOCK_DEPS)

uint32_t modem_clock_get_module_deps(shared_periph_module_t module)
{
    uint32_t deps = 0;
    switch (module) {
        case PERIPH_ANA_I2C_MASTER_MODULE:      deps = I2C_ANA_MST_CLOCK_DEPS;          break;
        case PERIPH_PHY_MODULE:                 deps = PHY_CLOCK_DEPS;                  break;
        case PERIPH_MODEM_ADC_COMMON_FE_MODULE: deps = MODEM_ADC_COMMON_FE_CLOCK_DEPS;  break;
#if SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
        case PERIPH_COEX_MODULE:                deps = COEXIST_CLOCK_DEPS;              break;
#endif
#if SOC_BT_SUPPORTED
        case PERIPH_BT_MODULE:                  deps = BLE_CLOCK_DEPS;                  break;
#endif
        case PERIPH_PHY_CALIBRATION_MODULE:     deps = PHY_CALIBRATION_CLOCK_DEPS;      break;
#if SOC_IEEE802154_SUPPORTED
        case PERIPH_IEEE802154_MODULE:          deps = IEEE802154_CLOCK_DEPS;           break;
#endif
#if SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
        case PERIPH_MODEM_ETM_MODULE:           deps = MODEM_ETM_CLOCK_DEPS;            break;
#endif
        default:
            assert(0);
    }
    return deps;
}

#if SOC_BT_SUPPORTED
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
#endif // SOC_BT_SUPPORTED

#if SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
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
#endif // SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED

#if SOC_IEEE802154_SUPPORTED
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
#endif // SOC_IEEE802154_SUPPORTED

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

#if ANA_I2C_MST_CLK_HAS_ROOT_GATING
static void IRAM_ATTR modem_clock_i2c_master_configure(modem_clock_context_t *ctx, bool enable)
{
    if (enable) {
        ANALOG_CLOCK_ENABLE();
    } else {
        ANALOG_CLOCK_DISABLE();
    }
}
#endif

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

#if ANA_I2C_MST_CLK_HAS_ROOT_GATING
static esp_err_t IRAM_ATTR modem_clock_i2c_master_check_enable(modem_clock_context_t *ctx)
{
    return ANALOG_CLOCK_IS_ENABLED() ? ESP_OK : ESP_FAIL;
}
#endif

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
#if ANA_I2C_MST_CLK_HAS_ROOT_GATING
// ANALOG_CLOCK_ENABLE/DISABLE has its own ref_cnt management.
    [MODEM_CLOCK_I2C_MASTER]            = { .refs = 0, .with_refcnt = false,    .configure = modem_clock_i2c_master_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_i2c_master_check_enable
#endif
    },
#endif
    [MODEM_CLOCK_ETM]                   = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_etm_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_etm_check_enable
#endif
    },
#if SOC_BT_SUPPORTED
    [MODEM_CLOCK_BLE_MAC]               = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_ble_mac_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_ble_mac_check_enable
#endif
    },
#endif
#if SOC_IEEE802154_SUPPORTED || SOC_BT_SUPPORTED
    [MODEM_CLOCK_BT_I154_COMMON_BB]     = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_ble_i154_bb_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_ble_i154_bb_check_enable
#endif
    },
#endif
#if SOC_IEEE802154_SUPPORTED
    [MODEM_CLOCK_802154_MAC]            = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_ieee802154_mac_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_ieee802154_mac_check_enable
#endif
    },
#endif
    [MODEM_CLOCK_DATADUMP]              = { .refs = 0, .with_refcnt = true,     .configure = modem_clock_data_dump_configure
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        , .check_enable = modem_clock_data_dump_check_enable
#endif
    }
};
