/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdatomic.h>
#include "sdkconfig.h"
#if CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_pm.h"
#include "driver/ana_cmpr_types.h"
#include "soc/soc_caps.h"
#include "hal/ana_cmpr_ll.h"
#include "hal/ana_cmpr_periph.h"

#define TAG "ana_cmpr"

#if CONFIG_ANA_CMPR_OBJ_CACHE_SAFE
#define ANA_CMPR_MEM_ALLOC_CAPS         (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define ANA_CMPR_MEM_ALLOC_CAPS         MALLOC_CAP_DEFAULT
#endif

#if CONFIG_ANA_CMPR_ISR_CACHE_SAFE
// the interrupt source of Analog Comparator is marked as shared because
// it may share with other GPIO interrupt events
// it may share with different analog comparator units
#define ANA_CMPR_INTR_FLAG              (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED)
#else
#define ANA_CMPR_INTR_FLAG              (ESP_INTR_FLAG_SHARED)
#endif

#define ANA_CMPR_ALLOW_INTR_PRIORITY_MASK ESP_INTR_FLAG_LOWMED

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ANA_CMPR_FSM_INIT,        // Comparator is in the initialization stage, not enabled yet
    ANA_CMPR_FSM_ENABLE,      // Comparator is enabled
    ANA_CMPR_FSM_WAIT,        // Comparator is in the middle of state change, so busy, other operations should wait
} ana_cmpr_fsm_t;

typedef struct ana_cmpr_t ana_cmpr_t;

typedef struct ana_cmpr_ref_chan_t {
    ana_cmpr_ref_source_t       ref_src;  // the reference source type of this reference channel, internal or external
    // for internal reference channel
    ana_cmpr_ref_voltage_t      ref_volt;
    ana_cmpr_ref_hys_t          ref_hys_level;
    // for external reference channel
    int                         gpio_num;
    uint32_t                    pad_id;   // the pad id corresponding to the gpio_num
} ana_cmpr_ref_chan_t;

typedef struct ana_cmpr_src_chan_t {
    uint8_t                     chan_id;  // there're multiple source channels in the analog comparator unit
    ana_cmpr_cross_type_t       cross_type;
    int                         gpio_num;
    uint32_t                    pad_id;   // the pad id corresponding to the gpio_num
} ana_cmpr_src_chan_t;

struct ana_cmpr_t {
    int                         unit_id;            // Analog comparator unit id
    analog_cmpr_dev_t           *dev;               // Analog comparator unit device address
    _Atomic ana_cmpr_fsm_t      fsm;                // The state machine of the Analog Comparator unit
    ana_cmpr_event_callbacks_t  cbs;                // The callback group that set by user
    void                        *user_data;         // User data that passed to the callbacks
    intr_handle_t               intr_handle;        // Interrupt handle
    uint32_t                    intr_mask;          // Interrupt mask
    int                         intr_priority;      // Interrupt priority
    uint32_t                    src_clk_freq_hz;    // Source clock frequency of the Analog Comparator unit
    ana_cmpr_src_chan_t         src_chans[ANALOG_CMPR_LL_GET(SRC_CHANNEL_NUM)]; // The source channel objects in the unit
    ana_cmpr_ref_chan_t         ref_chan;           // The reference channel object in the unit
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t        pm_lock;            // The Power Management lock that used to avoid unexpected power down of the clock domain
#endif
};

#ifdef __cplusplus
}
#endif
