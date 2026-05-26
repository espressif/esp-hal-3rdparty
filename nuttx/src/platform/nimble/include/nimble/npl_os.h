/*
 * SPDX-FileCopyrightText: 2015-2022 The Apache Software Foundation (ASF)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2019-2022 Espressif Systems (Shanghai) CO LTD
 */

#ifndef _NPL_OS_H_
#define _NPL_OS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#include "platform/os.h"
#include "esp_timer.h"

#if ((defined(CONFIG_BT_NIMBLE_USE_ESP_TIMER) && CONFIG_BT_NIMBLE_USE_ESP_TIMER) || \
     (defined(CONFIG_BT_LE_USE_ESP_TIMER) && CONFIG_BT_LE_USE_ESP_TIMER))
#  define BLE_NPL_USE_ESP_TIMER (1)
#else
#  define BLE_NPL_USE_ESP_TIMER (0)
#endif

typedef struct {
    uint16_t evt_count;
    uint16_t evtq_count;
    uint16_t co_count;
    uint16_t sem_count;
    uint16_t mutex_count;
} ble_npl_count_info_t;

typedef void ble_npl_event_fn(struct ble_npl_event *ev);

struct ble_npl_event_os {
    bool queued;
    ble_npl_event_fn *fn;
    void *arg;
};

struct ble_npl_eventq_os {
    esp_os_queue_handle_t q;
};

struct ble_npl_callout_os {
#if BLE_NPL_USE_ESP_TIMER
    esp_timer_handle_t handle;
#else
    void *handle;
#endif
    struct ble_npl_eventq *evq;
    struct ble_npl_event ev;
};

struct ble_npl_mutex_os {
    esp_os_recursive_mutex_t handle;
};

struct ble_npl_sem_os {
    esp_os_sem_t handle;
};

struct ble_npl_eventq *npl_os_eventq_dflt_get(void);

struct ble_npl_event *npl_os_eventq_get(struct ble_npl_eventq *evq,
                                        ble_npl_time_t tmo);

void npl_os_eventq_put(struct ble_npl_eventq *evq, struct ble_npl_event *ev);

void npl_os_eventq_remove(struct ble_npl_eventq *evq, struct ble_npl_event *ev);

ble_npl_error_t npl_os_mutex_init(struct ble_npl_mutex *mu);
ble_npl_error_t npl_os_mutex_deinit(struct ble_npl_mutex *mu);

ble_npl_error_t npl_os_mutex_pend(struct ble_npl_mutex *mu,
                                  ble_npl_time_t timeout);

ble_npl_error_t npl_os_mutex_release(struct ble_npl_mutex *mu);

ble_npl_error_t npl_os_sem_init(struct ble_npl_sem *sem, uint16_t tokens);
ble_npl_error_t npl_os_sem_deinit(struct ble_npl_sem *sem);

ble_npl_error_t npl_os_sem_pend(struct ble_npl_sem *sem,
                                ble_npl_time_t timeout);

ble_npl_error_t npl_os_sem_release(struct ble_npl_sem *sem);

int npl_os_callout_init(struct ble_npl_callout *co, struct ble_npl_eventq *evq,
                        ble_npl_event_fn *ev_cb, void *ev_arg);

void npl_os_callout_deinit(struct ble_npl_callout *co);

void npl_os_callout_stop(struct ble_npl_callout *co);

bool npl_os_callout_is_active(struct ble_npl_callout *co);

ble_npl_time_t npl_os_callout_get_ticks(struct ble_npl_callout *co);

ble_npl_error_t npl_os_callout_reset(struct ble_npl_callout *co,
                                     ble_npl_time_t ticks);

ble_npl_time_t npl_os_callout_remaining_ticks(struct ble_npl_callout *co,
                                              ble_npl_time_t now);

ble_npl_error_t npl_os_time_ms_to_ticks(uint32_t ms, ble_npl_time_t *out_ticks);

ble_npl_error_t npl_os_time_ticks_to_ms(ble_npl_time_t ticks, uint32_t *out_ms);

uint32_t npl_os_hw_enter_critical(void);

void npl_os_hw_exit_critical(uint32_t ctx);

void npl_os_funcs_init(void);
void npl_os_funcs_deinit(void);
int npl_os_mempool_init(void);
void npl_os_mempool_deinit(void);
struct npl_funcs_t *npl_os_funcs_get(void);
int npl_os_set_controller_npl_info(ble_npl_count_info_t *ctrl_npl_info);

#ifdef __cplusplus
}
#endif

#endif /* _NPL_OS_H_ */
