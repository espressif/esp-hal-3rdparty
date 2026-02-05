/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/pmu_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable analog I2C master clock
 */
static inline __attribute__((always_inline)) void _regi2c_ctrl_ll_master_enable_clock(bool en)
{
    // TODO: [ESP32S31] IDF-14680
}

// LPPERI.clk_en is a shared register, so this function must be used in an atomic way
#define regi2c_ctrl_ll_master_enable_clock(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _regi2c_ctrl_ll_master_enable_clock(__VA_ARGS__)

/**
 * @brief Check whether analog I2C master clock is enabled
 */
static inline __attribute__((always_inline)) bool regi2c_ctrl_ll_master_is_clock_enabled(void)
{
    return 0;// TODO: [ESP32S31] IDF-14680
}

/**
 * @brief Reset analog I2C master
 */
static inline __attribute__((always_inline)) void _regi2c_ctrl_ll_master_reset(void)
{
    // TODO: [ESP32S31] IDF-14680
}

// LPPERI.reset_en is a shared register, so this function must be used in an atomic way
#define regi2c_ctrl_ll_master_reset(...) (void)__DECLARE_RCC_RC_ATOMIC_ENV; _regi2c_ctrl_ll_master_reset(__VA_ARGS__)

/**
 * @brief Configure analog I2C master clock
 */
static inline __attribute__((always_inline)) void regi2c_ctrl_ll_master_configure_clock(void)
{
    // TODO: [ESP32S31] IDF-14680
}

/**
 * @brief Enable the I2C internal bus to do I2C read/write operation to the SAR_ADC and TSENS registers
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_enable(void)
{
    // TODO: [ESP32S31] IDF-14680
}

/**
 * @brief Disable the I2C internal bus to do I2C read/write operation to the SAR_ADC register
 */
static inline void regi2c_ctrl_ll_i2c_sar_periph_disable(void)
{
    // TODO: [ESP32S31] IDF-14680
}

#ifdef __cplusplus
}
#endif
