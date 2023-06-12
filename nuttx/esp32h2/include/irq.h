/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <nuttx/config.h>

#define PMU_INTR_SOURCE                     0
#define EFUSE_INTR_SOURCE                   1
#define LP_RTC_TIMER_INTR_SOURCE            2
#define LP_BLE_TIMER_INTR_SOURCE            3
#define LP_WDT_INTR_SOURCE                  4
#define LP_PERI_TIMEOUT_INTR_SOURCE         5
#define LP_APM_M0_INTR_SOURCE               6
#define FROM_CPU_INTR0_SOURCE               7
#define FROM_CPU_INTR1_SOURCE               8
#define FROM_CPU_INTR2_SOURCE               9
#define FROM_CPU_INTR3_SOURCE               10
#define ASSIST_DEBUG_INTR_SOURCE            11
#define TRACE_INTR_SOURCE                   12
#define CACHE_INTR_SOURCE                   13
#define CPU_PERI_TIMEOUT_INTR_SOURCE        14
#define BT_MAC_INTR_SOURCE                  15
#define BT_BB_INTR_SOURCE                   16
#define BT_BB_NMI_SOURCE                    17
#define COEX_INTR_SOURCE                    18
#define BLE_TIMER_INTR_SOURCE               19
#define BLE_SEC_INTR_SOURCE                 20
#define ZB_MAC_INTR_SOURCE                  21
#define GPIO_INTR_SOURCE                    22
#define GPIO_NMI_SOURCE                     23
#define PAU_INTR_SOURCE                     24
#define HP_PERI_TIMEOUT_INTR_SOURCE         25
#define HP_APM_M0_INTR_SOURCE               26
#define HP_APM_M1_INTR_SOURCE               27
#define HP_APM_M2_INTR_SOURCE               28
#define HP_APM_M3_INTR_SOURCE               29
#define MSPI_INTR_SOURCE                    30
#define I2S1_INTR_SOURCE                    31
#define UHCI0_INTR_SOURCE                   32
#define UART0_INTR_SOURCE                   33
#define UART1_INTR_SOURCE                   34
#define LEDC_INTR_SOURCE                    35
#define TWAI0_INTR_SOURCE                   36
#define USB_SERIAL_JTAG_INTR_SOURCE         37
#define RMT_INTR_SOURCE                     38
#define I2C_EXT0_INTR_SOURCE                39
#define I2C_EXT1_INTR_SOURCE                40
#define TG0_T0_LEVEL_INTR_SOURCE            41
#define TG0_WDT_LEVEL_INTR_SOURCE           42
#define TG1_T0_LEVEL_INTR_SOURCE            43
#define TG1_WDT_LEVEL_INTR_SOURCE           44
#define SYSTIMER_TARGET0_EDGE_INTR_SOURCE   45
#define SYSTIMER_TARGET1_EDGE_INTR_SOURCE   46
#define SYSTIMER_TARGET2_EDGE_INTR_SOURCE   47
#define APB_ADC_INTR_SOURCE                 48
#define MCPWM0_INTR_SOURCE                  49
#define PCNT_INTR_SOURCE                    50
#define PARL_IO_TX_INTR_SOURCE              51
#define PARL_IO_RX_INTR_SOURCE              52
#define DMA_IN_CH0_INTR_SOURCE              53
#define DMA_IN_CH1_INTR_SOURCE              54
#define DMA_IN_CH2_INTR_SOURCE              55
#define DMA_OUT_CH0_INTR_SOURCE             56
#define DMA_OUT_CH1_INTR_SOURCE             57
#define DMA_OUT_CH2_INTR_SOURCE             58
#define GSPI2_INTR_SOURCE                   59
#define AES_INTR_SOURCE                     60
#define SHA_INTR_SOURCE                     61
#define RSA_INTR_SOURCE                     62
#define ECC_INTR_SOURCE                     63
#define ECDSA_INTR_SOURCE                   64

/* Total number of peripherals interrupt sources */

#define ESP_NSOURCES                        65

/* CPU Interrupts */

#define ESP_NCPUINTS               32

/* The following interrupts are reserved:
 *   - 0, 3, 4, and 7 are used by the CPU for core-local interrupts (CLINT);
 *   - 1 for Wi-Fi;
 *   - 5 and 8 for Bluetooth;
 *   - 6 for "permanently disabled interrupt
 */

#define ESP_CPUINT_PERIPHSET       0xfffffe04

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
