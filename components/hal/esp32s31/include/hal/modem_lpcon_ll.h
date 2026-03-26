/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for ESP32-S31 MODEM LPCON register operations

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((always_inline))
static inline void modem_lpcon_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.modem_ctrl0.reg_modem_clk_en = enable;
}

#ifdef __cplusplus
}
#endif
