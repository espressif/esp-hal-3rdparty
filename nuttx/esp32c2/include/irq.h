/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <nuttx/config.h>

#define WIFI_MAC_INTR_SOURCE                0
#define WIFI_MAC_NMI_SOURCE                 1
#define WIFI_PWR_INTR_SOURCE                2
#define WIFI_BB_INTR_SOURCE                 3
#define BT_MAC_INTR_SOURCE                  4
#define BT_BB_INTR_SOURCE                   5
#define BT_BB_NMI_SOURCE                    6
#define LP_TIMER_INTR_SOURCE                7
#define COEX_INTR_SOURCE                    8
#define BLE_TIMER_INTR_SOURCE               9
#define BLE_SEC_INTR_SOURCE                 10
#define I2C_MASTER_INTR_SOURCE              11
#define APB_CTRL_INTR_SOURCE                12
#define GPIO_INTR_SOURCE                    13
#define GPIO_NMI_SOURCE                     14
#define SPI1_INTR_SOURCE                    15
#define SPI2_INTR_SOURCE                    16
#define UART0_INTR_SOURCE                   17
#define UART1_INTR_SOURCE                   18
#define LEDC_INTR_SOURCE                    19
#define EFUSE_INTR_SOURCE                   20
#define RTC_CORE_INTR_SOURCE                21
#define I2C_EXT0_INTR_SOURCE                22
#define TG0_T0_LEVEL_INTR_SOURCE            23
#define TG0_WDT_LEVEL_INTR_SOURCE           24
#define CACHE_IA_INTR_SOURCE                25
#define SYSTIMER_TARGET0_EDGE_INTR_SOURCE   26
#define SYSTIMER_TARGET1_EDGE_INTR_SOURCE   27
#define SYSTIMER_TARGET2_EDGE_INTR_SOURCE   28
#define SPI_MEM_REJECT_CACHE_INTR_SOURCE    29
#define ICACHE_PRELOAD0_INTR_SOURCE         30
#define ICACHE_SYNC0_INTR_SOURCE            31
#define APB_ADC_INTR_SOURCE                 32
#define DMA_CH0_INTR_SOURCE                 33
#define SHA_INTR_SOURCE                     34
#define ECC_INTR_SOURCE                     35
#define FROM_CPU_INTR0_SOURCE               36
#define FROM_CPU_INTR1_SOURCE               37
#define FROM_CPU_INTR2_SOURCE               38
#define FROM_CPU_INTR3_SOURCE               39
#define ASSIST_DEBUG_INTR_SOURCE            40
#define CORE0_PIF_PMS_SIZE_INTR_SOURCE      41
#define CACHE_CORE0_ACS_INTR_SOURCE         42

/* Total number of peripheral interrupt sources */

#define ESP_NSOURCES                        43

/* CPU Interrupts */

#define ESP_NCPUINTS               32

/* The following interrupts are reserved:
 *   - 0 (not available);
 *   - 1 for Wi-Fi;
 *   - 5 and 8 for Bluetooth;
 *   - 6 for "permanently disabled interrupt
 */

#define ESP_CPUINT_PERIPHSET       0xfffffe9c

/* IRQ numbers. */

/* ecall is dispatched like normal interrupts.  It occupies an IRQ number. */

#define RISCV_NIRQ_INTERRUPTS      16                                       /* Number of RISC-V dispatched interrupts. */
#define ESP_NIRQ_DEMUX             1                                        /* Number of IRQs for the demapping interrupt */
#define ESP_IRQ_FIRSTPERIPH        RISCV_NIRQ_INTERRUPTS + ESP_NIRQ_DEMUX   /* First peripheral IRQ number */

/* Special IRQ for the demapping interrupt */

#define ESP_IRQ_DEMUX              RISCV_NIRQ_INTERRUPTS

/* IRQ numbers for peripheral interrupts coming through the Interrupt
 * Matrix.
 */

#define ESP_IRQ2SOURCE(irq)        ((irq) - ESP_IRQ_FIRSTPERIPH)
#define ESP_SOURCE2IRQ(id)         ((id) + ESP_IRQ_FIRSTPERIPH)

/* Peripheral IRQs */

#define ESP_IRQ_WIFI_MAC                ESP_SOURCE2IRQ(WIFI_MAC_INTR_SOURCE)
#define ESP_IRQ_WIFI_MAC_NMI            ESP_SOURCE2IRQ(WIFI_MAC_NMI_SOURCE)
#define ESP_IRQ_WIFI_PWR                ESP_SOURCE2IRQ(WIFI_PWR_INTR_SOURCE)
#define ESP_IRQ_WIFI_BB                 ESP_SOURCE2IRQ(WIFI_BB_INTR_SOURCE)
#define ESP_IRQ_BT_MAC                  ESP_SOURCE2IRQ(BT_MAC_INTR_SOURCE)
#define ESP_IRQ_BT_BB                   ESP_SOURCE2IRQ(BT_BB_INTR_SOURCE)
#define ESP_IRQ_BT_BB_NMI               ESP_SOURCE2IRQ(BT_BB_NMI_SOURCE)
#define ESP_IRQ_LP_TIMER                ESP_SOURCE2IRQ(LP_TIMER_INTR_SOURCE)
#define ESP_IRQ_COEX                    ESP_SOURCE2IRQ(COEX_INTR_SOURCE)
#define ESP_IRQ_BLE_TIMER               ESP_SOURCE2IRQ(BLE_TIMER_INTR_SOURCE)
#define ESP_IRQ_BLE_SEC                 ESP_SOURCE2IRQ(BLE_SEC_INTR_SOURCE)
#define ESP_IRQ_I2C_MASTER              ESP_SOURCE2IRQ(I2C_MASTER_INTR_SOURCE)
#define ESP_IRQ_APB_CTRL                ESP_SOURCE2IRQ(APB_CTRL_INTR_SOURCE)
#define ESP_IRQ_GPIO                    ESP_SOURCE2IRQ(GPIO_INTR_SOURCE)
#define ESP_IRQ_GPIO_NMI                ESP_SOURCE2IRQ(GPIO_NMI_SOURCE)
#define ESP_IRQ_SPI1                    ESP_SOURCE2IRQ(SPI1_INTR_SOURCE)
#define ESP_IRQ_SPI2                    ESP_SOURCE2IRQ(SPI2_INTR_SOURCE)
#define ESP_IRQ_UART0                   ESP_SOURCE2IRQ(UART0_INTR_SOURCE)
#define ESP_IRQ_UART1                   ESP_SOURCE2IRQ(UART1_INTR_SOURCE)
#define ESP_IRQ_LEDC                    ESP_SOURCE2IRQ(LEDC_INTR_SOURCE)
#define ESP_IRQ_EFUSE                   ESP_SOURCE2IRQ(EFUSE_INTR_SOURCE)
#define ESP_IRQ_RTC_CORE                ESP_SOURCE2IRQ(RTC_CORE_INTR_SOURCE)
#define ESP_IRQ_I2C_EXT0                ESP_SOURCE2IRQ(I2C_EXT0_INTR_SOURCE)
#define ESP_IRQ_TG0_T0_LEVEL            ESP_SOURCE2IRQ(TG0_T0_LEVEL_INTR_SOURCE)
#define ESP_IRQ_TG0_WDT_LEVEL           ESP_SOURCE2IRQ(TG0_WDT_LEVEL_INTR_SOURCE)
#define ESP_IRQ_CACHE_IA                ESP_SOURCE2IRQ(CACHE_IA_INTR_SOURCE)
#define ESP_IRQ_SYSTIMER_TARGET0_EDGE   ESP_SOURCE2IRQ(SYSTIMER_TARGET0_EDGE_INTR_SOURCE)
#define ESP_IRQ_SYSTIMER_TARGET1_EDGE   ESP_SOURCE2IRQ(SYSTIMER_TARGET1_EDGE_INTR_SOURCE)
#define ESP_IRQ_SYSTIMER_TARGET2_EDGE   ESP_SOURCE2IRQ(SYSTIMER_TARGET2_EDGE_INTR_SOURCE)
#define ESP_IRQ_SPI_MEM_REJECT_CACHE    ESP_SOURCE2IRQ(SPI_MEM_REJECT_CACHE_INTR_SOURCE)
#define ESP_IRQ_ICACHE_PRELOAD0         ESP_SOURCE2IRQ(ICACHE_PRELOAD0_INTR_SOURCE)
#define ESP_IRQ_ICACHE_SYNC0            ESP_SOURCE2IRQ(ICACHE_SYNC0_INTR_SOURCE)
#define ESP_IRQ_APB_ADC                 ESP_SOURCE2IRQ(APB_ADC_INTR_SOURCE)
#define ESP_IRQ_DMA_CH0                 ESP_SOURCE2IRQ(DMA_CH0_INTR_SOURCE)
#define ESP_IRQ_SHA                     ESP_SOURCE2IRQ(SHA_INTR_SOURCE)
#define ESP_IRQ_ECC                     ESP_SOURCE2IRQ(ECC_INTR_SOURCE)
#define ESP_IRQ_FROM_CPU_INTR0          ESP_SOURCE2IRQ(FROM_CPU_INTR0_SOURCE)
#define ESP_IRQ_FROM_CPU_INTR1          ESP_SOURCE2IRQ(FROM_CPU_INTR1_SOURCE)
#define ESP_IRQ_FROM_CPU_INTR2          ESP_SOURCE2IRQ(FROM_CPU_INTR2_SOURCE)
#define ESP_IRQ_FROM_CPU_INTR3          ESP_SOURCE2IRQ(FROM_CPU_INTR3_SOURCE)
#define ESP_IRQ_ASSIST_DEBUG            ESP_SOURCE2IRQ(ASSIST_DEBUG_INTR_SOURCE)
#define ESP_IRQ_CORE0_PIF_PMS_SIZE      ESP_SOURCE2IRQ(CORE0_PIF_PMS_SIZE_INTR_SOURCE)
#define ESP_IRQ_CACHE_CORE0_ACS         ESP_SOURCE2IRQ(CACHE_CORE0_ACS_INTR_SOURCE)

#define ESP_NIRQ_PERIPH                 ESP_NSOURCES

/* Second level GPIO interrupts.  GPIO interrupts are decoded and dispatched
 * as a second level of decoding:  The first level dispatches to the GPIO
 * interrupt handler.  The second to the decoded GPIO interrupt handler.
 */

#ifdef CONFIG_ESPRESSIF_GPIO_IRQ
#  define ESP_NIRQ_GPIO             22
#  define ESP_FIRST_GPIOIRQ         (RISCV_NIRQ_INTERRUPTS + ESP_NIRQ_PERIPH)
#  define ESP_LAST_GPIOIRQ          (ESP_FIRST_GPIOIRQ + ESP_NIRQ_GPIO - 1)
#  define ESP_PIN2IRQ(p)            ((p) + ESP_FIRST_GPIOIRQ)
#  define ESP_IRQ2PIN(i)            ((i) - ESP_FIRST_GPIOIRQ)
#else
#  define ESP_NIRQ_GPIO             0
#endif

/* Total number of IRQs: ecall + Number of peripheral IRQs + GPIOs IRQs. */

#define NR_IRQS  (RISCV_NIRQ_INTERRUPTS + ESP_NIRQ_DEMUX + ESP_NIRQ_PERIPH + ESP_NIRQ_GPIO)

#if CONFIG_ARCH_NUSER_INTERRUPTS < (ESP_IRQ_DEMUX + 1)
#  error "CONFIG_ARCH_NUSER_INTERRUPTS is less than ESP_IRQ_DEMUX + 1. Please increase CONFIG_ARCH_NUSER_INTERRUPTS to at least ESP_IRQ_DEMUX + 1."
#endif
