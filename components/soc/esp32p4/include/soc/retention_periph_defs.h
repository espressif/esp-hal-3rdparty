/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum periph_retention_module {
    SLEEP_RETENTION_MODULE_MIN          = 0,
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_CLOCK_SYSTEM = 1,
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_SYS_PERIPH   = 2,
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_TG0_WDT      = 3,
    SLEEP_RETENTION_MODULE_TG1_WDT      = 4,
    SLEEP_RETENTION_MODULE_TG0_TIMER    = 5,
    SLEEP_RETENTION_MODULE_TG1_TIMER    = 6,
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_UART0        = 7,
    SLEEP_RETENTION_MODULE_UART1        = 8,
    SLEEP_RETENTION_MODULE_UART2        = 9,
    SLEEP_RETENTION_MODULE_UART3        = 10,
    SLEEP_RETENTION_MODULE_UART4        = 11,

    SLEEP_RETENTION_MODULE_MAX          = 31
} periph_retention_module_t;

typedef enum periph_retention_module_bitmap {
    /* clock module, which includes system and modem */
    SLEEP_RETENTION_MODULE_BM_CLOCK_SYSTEM = BIT(SLEEP_RETENTION_MODULE_CLOCK_SYSTEM),
    /* digital peripheral module, which includes Interrupt Matrix, HP_SYSTEM,
     * TEE, APM, UART, Timer Group, IOMUX, SPIMEM, SysTimer, etc.. */
    SLEEP_RETENTION_MODULE_BM_SYS_PERIPH   = BIT(SLEEP_RETENTION_MODULE_SYS_PERIPH),
    /* Timer Group by target*/
    SLEEP_RETENTION_MODULE_BM_TG0_WDT      = BIT(SLEEP_RETENTION_MODULE_TG0_WDT),
    SLEEP_RETENTION_MODULE_BM_TG1_WDT      = BIT(SLEEP_RETENTION_MODULE_TG1_WDT),
    SLEEP_RETENTION_MODULE_BM_TG0_TIMER    = BIT(SLEEP_RETENTION_MODULE_TG0_TIMER),
    SLEEP_RETENTION_MODULE_BM_TG1_TIMER    = BIT(SLEEP_RETENTION_MODULE_TG1_TIMER),
    /* MISC Peripherals */
    SLEEP_RETENTION_MODULE_BM_UART0        = BIT(SLEEP_RETENTION_MODULE_UART0),
    SLEEP_RETENTION_MODULE_BM_UART1        = BIT(SLEEP_RETENTION_MODULE_UART1),
    SLEEP_RETENTION_MODULE_BM_UART2        = BIT(SLEEP_RETENTION_MODULE_UART2),
    SLEEP_RETENTION_MODULE_BM_UART3        = BIT(SLEEP_RETENTION_MODULE_UART3),
    SLEEP_RETENTION_MODULE_BM_UART4        = BIT(SLEEP_RETENTION_MODULE_UART4),

    SLEEP_RETENTION_MODULE_BM_ALL = (uint32_t)-1
} periph_retention_module_bitmap_t;

#define TOP_DOMAIN_PERIPHERALS_BM  (SLEEP_RETENTION_MODULE_BM_CLOCK_SYSTEM \
                                    | SLEEP_RETENTION_MODULE_BM_SYS_PERIPH \
                                    | SLEEP_RETENTION_MODULE_BM_TG0_WDT    \
                                    | SLEEP_RETENTION_MODULE_BM_TG1_WDT    \
                                    | SLEEP_RETENTION_MODULE_BM_TG0_TIMER  \
                                    | SLEEP_RETENTION_MODULE_BM_TG1_TIMER  \
                                    | SLEEP_RETENTION_MODULE_BM_UART0      \
                                    | SLEEP_RETENTION_MODULE_BM_UART1      \
                                    | SLEEP_RETENTION_MODULE_BM_UART2      \
                                    | SLEEP_RETENTION_MODULE_BM_UART3      \
                                    | SLEEP_RETENTION_MODULE_BM_UART4      \
                                   )

#ifdef __cplusplus
}
#endif
