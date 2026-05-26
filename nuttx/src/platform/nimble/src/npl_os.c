/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <nuttx/config.h>
#include <nuttx/arch.h>
#include <nuttx/mqueue.h>
#include <nuttx/semaphore.h>

#include "nimble/nimble_npl.h"
#include "nimble/npl_os.h"
#include "esp_private/critical_section.h"
#include "soc/soc_caps.h"
#include "os/os_mempool.h"
#include "esp_bt.h"
#include "esp_heap_caps.h"
#include "bt_osi_mem.h"
#include "esp_timer.h"

#if !SOC_ESP_NIMBLE_CONTROLLER
#  error "SOC_ESP_NIMBLE_CONTROLLER is required for BLE NPL"
#endif

#define BLE_HOST_CO_COUNT    (0)
#define BLE_HOST_EV_COUNT    (0)
#define BLE_HOST_EVQ_COUNT   (0)
#define BLE_HOST_SEM_COUNT   (0)
#define BLE_HOST_MUTEX_COUNT (0)

DEFINE_CRIT_SECTION_LOCK_STATIC(ble_port_lock);

static ble_npl_count_info_t g_ctrl_npl_info;
static uint16_t ble_os_total_event_cnt;
static uint8_t hw_critical_state_status;

static inline bool IRAM_ATTR in_isr(void)
{
  return up_interrupt_context();
}

static bool npl_os_queue_is_empty(esp_os_queue_handle_t q)
{
  struct mq_attr attr;
  FAR struct mq_adpt *mq = q;

  if (file_mq_getattr(&mq->mq, &attr) < 0)
    {
      return true;
    }

  return attr.mq_curmsgs == 0;
}

bool IRAM_ATTR npl_os_os_started(void)
{
  return esp_os_scheduler_started();
}

void *IRAM_ATTR npl_os_get_current_task_id(void)
{
  return (void *)(uintptr_t)esp_os_task_get_current_handle();
}

void IRAM_ATTR npl_os_event_init(struct ble_npl_event *ev, ble_npl_event_fn *fn,
                                    void *arg)
{
  struct ble_npl_event_os *event;

  if (!ev->event)
    {
      ev->event = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_event_os));
    }

  event = (struct ble_npl_event_os *)ev->event;
  BLE_LL_ASSERT(event);

  memset(event, 0, sizeof(*event));
  event->fn = fn;
  event->arg = arg;
}

void IRAM_ATTR npl_os_event_deinit(struct ble_npl_event *ev)
{
  if (!ev->event)
    {
      return;
    }

  bt_osi_mem_free_internal(ev->event);
  ev->event = NULL;
}

void IRAM_ATTR npl_os_event_reset(struct ble_npl_event *ev)
{
  struct ble_npl_event_os *event = (struct ble_npl_event_os *)ev->event;

  BLE_LL_ASSERT(event);
  event->queued = false;
}

void npl_os_eventq_init(struct ble_npl_eventq *evq)
{
  struct ble_npl_eventq_os *eventq;

  if (!evq->eventq)
    {
      evq->eventq = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_eventq_os));
      eventq = (struct ble_npl_eventq_os *)evq->eventq;
      BLE_LL_ASSERT(eventq);
      memset(eventq, 0, sizeof(*eventq));
      eventq->q = esp_os_queue_create_with_caps(ble_os_total_event_cnt,
                                               sizeof(struct ble_npl_event *),
                                               MALLOC_CAP_INTERNAL |
                                               MALLOC_CAP_8BIT);
      BLE_LL_ASSERT(eventq->q);
    }
}

void npl_os_eventq_deinit(struct ble_npl_eventq *evq)
{
  struct ble_npl_eventq_os *eventq =
    (struct ble_npl_eventq_os *)evq->eventq;

  if (!eventq)
    {
      return;
    }

  esp_os_queue_delete(eventq->q);
  bt_osi_mem_free_internal(eventq);
  evq->eventq = NULL;
}

void IRAM_ATTR npl_os_callout_mem_reset(struct ble_npl_callout *co)
{
  struct ble_npl_callout_os *callout = (struct ble_npl_callout_os *)co->co;

  BLE_LL_ASSERT(callout);
  BLE_LL_ASSERT(callout->handle);
  ble_npl_event_reset(&callout->ev);
}

struct ble_npl_event *IRAM_ATTR npl_os_eventq_get(struct ble_npl_eventq *evq,
                                                   ble_npl_time_t tmo)
{
  struct ble_npl_event *ev = NULL;
  struct ble_npl_eventq_os *eventq =
    (struct ble_npl_eventq_os *)evq->eventq;
  esp_err_t ret;

  if (in_isr())
    {
      BLE_LL_ASSERT(tmo == 0);
      ret = esp_os_queue_receive_from_isr(eventq->q, &ev, NULL);
    }
  else
    {
      ret = esp_os_queue_receive(eventq->q, &ev, tmo);
    }

  if (ret == ESP_OK && ev && ev->event)
    {
      struct ble_npl_event_os *event =
        (struct ble_npl_event_os *)ev->event;

      event->queued = false;
    }
  else
    {
      ev = NULL;
    }

  return ev;
}

void IRAM_ATTR npl_os_eventq_put(struct ble_npl_eventq *evq,
                                    struct ble_npl_event *ev)
{
  struct ble_npl_eventq_os *eventq =
    (struct ble_npl_eventq_os *)evq->eventq;
  struct ble_npl_event_os *event =
    (struct ble_npl_event_os *)ev->event;
  esp_err_t ret;

  if (event->queued)
    {
      return;
    }

  event->queued = true;

  if (in_isr())
    {
      ret = esp_os_queue_send_from_isr(eventq->q, &ev, NULL);
    }
  else
    {
      ret = esp_os_queue_send(eventq->q, &ev, portMAX_DELAY);
    }

  BLE_LL_ASSERT(ret == ESP_OK);
}

void IRAM_ATTR npl_os_eventq_put_to_front(struct ble_npl_eventq *evq,
                                           struct ble_npl_event *ev)
{
  /* NuttX message queues have no native push-front; requeue with ev first. */
  npl_os_eventq_remove(evq, ev);
  npl_os_eventq_put(evq, ev);
}

void IRAM_ATTR npl_os_eventq_remove(struct ble_npl_eventq *evq,
                                     struct ble_npl_event *ev)
{
  struct ble_npl_event *tmp_ev;
  struct ble_npl_eventq_os *eventq =
    (struct ble_npl_eventq_os *)evq->eventq;
  struct ble_npl_event_os *event =
    (struct ble_npl_event_os *)ev->event;
  esp_err_t ret;
  int i;
  int count;

  if (!event->queued)
    {
      return;
    }

  esp_os_enter_critical_safe(&ble_port_lock);

  count = 0;
  while (!npl_os_queue_is_empty(eventq->q))
    {
      count++;
      ret = esp_os_queue_receive(eventq->q, &tmp_ev, 0);
      BLE_LL_ASSERT(ret == ESP_OK);

      if (tmp_ev != ev)
        {
          ret = esp_os_queue_send(eventq->q, &tmp_ev, 0);
          BLE_LL_ASSERT(ret == ESP_OK);
        }
    }

  (void)count;

  esp_os_exit_critical_safe(&ble_port_lock);
  event->queued = false;
}

ble_npl_error_t npl_os_mutex_init(struct ble_npl_mutex *mu)
{
  struct ble_npl_mutex_os *mutex;

  if (!mu->mutex)
    {
      mu->mutex = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_mutex_os));
      mutex = (struct ble_npl_mutex_os *)mu->mutex;
      if (!mutex)
        {
          return BLE_NPL_INVALID_PARAM;
        }

      memset(mutex, 0, sizeof(*mutex));
      esp_os_create_recursive_mutex(&mutex->handle);
    }

  return BLE_NPL_OK;
}

ble_npl_error_t npl_os_mutex_deinit(struct ble_npl_mutex *mu)
{
  struct ble_npl_mutex_os *mutex = (struct ble_npl_mutex_os *)mu->mutex;

  if (!mutex)
    {
      return BLE_NPL_INVALID_PARAM;
    }

  esp_os_delete_recursive_mutex(&mutex->handle);
  bt_osi_mem_free_internal(mutex);
  mu->mutex = NULL;

  return BLE_NPL_OK;
}

void IRAM_ATTR npl_os_event_run(struct ble_npl_event *ev)
{
  struct ble_npl_event_os *event = (struct ble_npl_event_os *)ev->event;

  event->fn(ev);
}

bool IRAM_ATTR npl_os_eventq_is_empty(struct ble_npl_eventq *evq)
{
  struct ble_npl_eventq_os *eventq =
    (struct ble_npl_eventq_os *)evq->eventq;

  return npl_os_queue_is_empty(eventq->q);
}

bool IRAM_ATTR npl_os_event_is_queued(struct ble_npl_event *ev)
{
  struct ble_npl_event_os *event = (struct ble_npl_event_os *)ev->event;

  return event->queued;
}

void *IRAM_ATTR npl_os_event_get_arg(struct ble_npl_event *ev)
{
  struct ble_npl_event_os *event = (struct ble_npl_event_os *)ev->event;

  return event->arg;
}

void IRAM_ATTR npl_os_event_set_arg(struct ble_npl_event *ev, void *arg)
{
  struct ble_npl_event_os *event = (struct ble_npl_event_os *)ev->event;

  event->arg = arg;
}

ble_npl_error_t IRAM_ATTR npl_os_mutex_pend(struct ble_npl_mutex *mu,
                                             ble_npl_time_t timeout)
{
  struct ble_npl_mutex_os *mutex = (struct ble_npl_mutex_os *)mu->mutex;
  int ret;

  if (!mutex)
    {
      return BLE_NPL_INVALID_PARAM;
    }

  if (in_isr())
    {
      BLE_LL_ASSERT(0);
      return BLE_NPL_ERR_IN_ISR;
    }

  ret = nxrmutex_ticklock(&mutex->handle, timeout);

  return ret == 0 ? BLE_NPL_OK : BLE_NPL_TIMEOUT;
}

ble_npl_error_t IRAM_ATTR npl_os_mutex_release(struct ble_npl_mutex *mu)
{
  struct ble_npl_mutex_os *mutex = (struct ble_npl_mutex_os *)mu->mutex;

  if (!mutex)
    {
      return BLE_NPL_INVALID_PARAM;
    }

  if (in_isr())
    {
      BLE_LL_ASSERT(0);
      return BLE_NPL_ERR_IN_ISR;
    }

  esp_os_unlock_recursive_mutex(&mutex->handle);
  return BLE_NPL_OK;
}

ble_npl_error_t npl_os_sem_init(struct ble_npl_sem *sem, uint16_t tokens)
{
  struct ble_npl_sem_os *semaphore;

  if (!sem->sem)
    {
      sem->sem = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_sem_os));
      semaphore = (struct ble_npl_sem_os *)sem->sem;
      if (!semaphore)
        {
          return BLE_NPL_INVALID_PARAM;
        }

      memset(semaphore, 0, sizeof(*semaphore));
      esp_os_create_sem(&semaphore->handle);

      while (tokens-- > 0)
        {
          nxsem_post(&semaphore->handle);
        }
    }

  return BLE_NPL_OK;
}

ble_npl_error_t npl_os_sem_deinit(struct ble_npl_sem *sem)
{
  struct ble_npl_sem_os *semaphore = (struct ble_npl_sem_os *)sem->sem;

  if (!semaphore)
    {
      return BLE_NPL_INVALID_PARAM;
    }

  esp_os_destroy_sem(&semaphore->handle);
  bt_osi_mem_free_internal(semaphore);
  sem->sem = NULL;

  return BLE_NPL_OK;
}

ble_npl_error_t IRAM_ATTR npl_os_sem_pend(struct ble_npl_sem *sem,
                                           ble_npl_time_t timeout)
{
  struct ble_npl_sem_os *semaphore = (struct ble_npl_sem_os *)sem->sem;
  int ret;

  if (!semaphore)
    {
      return BLE_NPL_INVALID_PARAM;
    }

  if (in_isr())
    {
      BLE_LL_ASSERT(timeout == 0);
      ret = nxsem_trywait(&semaphore->handle);
    }
  else if (timeout == portMAX_DELAY)
    {
      ret = esp_os_wait_sem_timeout(&semaphore->handle, UINT32_MAX);
    }
  else
    {
      ret = esp_os_wait_sem_timeout(&semaphore->handle, timeout);
    }

  return ret == 0 ? BLE_NPL_OK : BLE_NPL_TIMEOUT;
}

ble_npl_error_t IRAM_ATTR npl_os_sem_release(struct ble_npl_sem *sem)
{
  struct ble_npl_sem_os *semaphore = (struct ble_npl_sem_os *)sem->sem;
  int ret;

  if (!semaphore)
    {
      return BLE_NPL_INVALID_PARAM;
    }

  if (in_isr())
    {
      ret = esp_os_post_sem_isr(&semaphore->handle, NULL);
    }
  else
    {
      ret = nxsem_post(&semaphore->handle);
    }

  BLE_LL_ASSERT(ret == 0);
  return BLE_NPL_OK;
}

uint16_t IRAM_ATTR npl_os_sem_get_count(struct ble_npl_sem *sem)
{
  struct ble_npl_sem_os *semaphore = (struct ble_npl_sem_os *)sem->sem;
  int val = 0;

  nxsem_get_value(&semaphore->handle, &val);
  return (uint16_t)val;
}

#if BLE_NPL_USE_ESP_TIMER
static void IRAM_ATTR ble_npl_event_fn_wrapper(void *arg)
{
  struct ble_npl_callout_os *callout = (struct ble_npl_callout_os *)arg;

  BLE_LL_ASSERT(callout);

  if (callout->evq)
    {
      ble_npl_eventq_put(callout->evq, &callout->ev);
    }
  else
    {
      struct ble_npl_event_os *event =
        (struct ble_npl_event_os *)callout->ev.event;

      event->fn(&callout->ev);
    }
}

static ble_npl_error_t esp_err_to_npl_error(esp_err_t err)
{
  switch (err)
    {
    case ESP_ERR_INVALID_ARG:
      return BLE_NPL_INVALID_PARAM;
    case ESP_ERR_INVALID_STATE:
      return BLE_NPL_EINVAL;
    case ESP_OK:
      return BLE_NPL_OK;
    default:
      return BLE_NPL_ERROR;
    }
}
#endif

int npl_os_callout_init(struct ble_npl_callout *co, struct ble_npl_eventq *evq,
                         ble_npl_event_fn *ev_cb, void *ev_arg)
{
  struct ble_npl_callout_os *callout;

  if (!co->co)
    {
      co->co = bt_osi_mem_malloc_internal(sizeof(struct ble_npl_callout_os));
      callout = (struct ble_npl_callout_os *)co->co;
      if (!callout)
        {
          return -1;
        }

      memset(callout, 0, sizeof(*callout));
      ble_npl_event_init(&callout->ev, ev_cb, ev_arg);
      callout->evq = evq;

#if BLE_NPL_USE_ESP_TIMER
      esp_timer_create_args_t create_args = {
        .callback = ble_npl_event_fn_wrapper,
        .arg = callout,
        .name = "nimble_timer"
      };

      if (esp_timer_create(&create_args, &callout->handle) != ESP_OK)
        {
          ble_npl_event_deinit(&callout->ev);
          bt_osi_mem_free_internal(callout);
          co->co = NULL;
          return -1;
        }
#endif
    }
  else
    {
      callout = (struct ble_npl_callout_os *)co->co;
      callout->evq = evq;
      ble_npl_event_init(&callout->ev, ev_cb, ev_arg);
    }

  return 0;
}

void npl_os_callout_deinit(struct ble_npl_callout *co)
{
  struct ble_npl_callout_os *callout = (struct ble_npl_callout_os *)co->co;

  if (!callout || !callout->handle)
    {
      return;
    }

  ble_npl_event_deinit(&callout->ev);
#if BLE_NPL_USE_ESP_TIMER
  esp_timer_stop(callout->handle);
  esp_timer_delete(callout->handle);
#endif
  bt_osi_mem_free_internal(callout);
  co->co = NULL;
  memset(co, 0, sizeof(*co));
}

ble_npl_error_t IRAM_ATTR npl_os_callout_reset(struct ble_npl_callout *co,
                                                ble_npl_time_t ticks)
{
  struct ble_npl_callout_os *callout = (struct ble_npl_callout_os *)co->co;

#if BLE_NPL_USE_ESP_TIMER
  esp_timer_stop(callout->handle);
  return esp_err_to_npl_error(esp_timer_start_once(callout->handle, ticks * 1000));
#else
  return BLE_NPL_ERROR;
#endif
}

void IRAM_ATTR npl_os_callout_stop(struct ble_npl_callout *co)
{
  struct ble_npl_callout_os *callout = (struct ble_npl_callout_os *)co->co;

  if (!callout)
    {
      return;
    }

#if BLE_NPL_USE_ESP_TIMER
  esp_timer_stop(callout->handle);
#endif
}

bool IRAM_ATTR npl_os_callout_is_active(struct ble_npl_callout *co)
{
  struct ble_npl_callout_os *callout = (struct ble_npl_callout_os *)co->co;

#if BLE_NPL_USE_ESP_TIMER
  return esp_timer_is_active(callout->handle);
#else
  return false;
#endif
}

ble_npl_time_t IRAM_ATTR npl_os_callout_get_ticks(struct ble_npl_callout *co)
{
  struct ble_npl_callout_os *callout = (struct ble_npl_callout_os *)co->co;
  uint32_t exp = 0;
#if BLE_NPL_USE_ESP_TIMER
  uint64_t expiry = 0;

  if (esp_timer_get_expiry_time(callout->handle, &expiry) == ESP_OK)
    {
      npl_os_time_ms_to_ticks((uint32_t)(expiry / 1000), &exp);
    }
#endif
  return exp;
}

ble_npl_time_t IRAM_ATTR npl_os_callout_remaining_ticks(struct ble_npl_callout *co,
                                                       ble_npl_time_t now)
{
  ble_npl_time_t exp = npl_os_callout_get_ticks(co);

  if (exp > now)
    {
      return exp - now;
    }

  return 0;
}

void IRAM_ATTR npl_os_callout_set_arg(struct ble_npl_callout *co, void *arg)
{
  struct ble_npl_callout_os *callout = (struct ble_npl_callout_os *)co->co;
  struct ble_npl_event_os *event =
    (struct ble_npl_event_os *)callout->ev.event;

  event->arg = arg;
}

uint32_t IRAM_ATTR npl_os_time_get(void)
{
#if BLE_NPL_USE_ESP_TIMER
  return esp_timer_get_time() / 1000;
#else
  return esp_os_task_get_tick_count();
#endif
}

ble_npl_error_t IRAM_ATTR npl_os_time_ms_to_ticks(uint32_t ms,
                                                   ble_npl_time_t *out_ticks)
{
  uint64_t ticks = ms;

  if (ticks > UINT32_MAX)
    {
      return BLE_NPL_EINVAL;
    }

  *out_ticks = (ble_npl_time_t)ticks;
  return BLE_NPL_OK;
}

ble_npl_error_t IRAM_ATTR npl_os_time_ticks_to_ms(ble_npl_time_t ticks,
                                                   uint32_t *out_ms)
{
  uint64_t ms = ticks;

  if (ms > UINT32_MAX)
    {
      return BLE_NPL_EINVAL;
    }

  *out_ms = (uint32_t)ms;
  return BLE_NPL_OK;
}

ble_npl_time_t IRAM_ATTR npl_os_time_ms_to_ticks32(uint32_t ms)
{
  return ms;
}

uint32_t IRAM_ATTR npl_os_time_ticks_to_ms32(ble_npl_time_t ticks)
{
  return ticks;
}

void IRAM_ATTR npl_os_time_delay(ble_npl_time_t ticks)
{
  esp_os_task_delay_adapter(ticks);
}

uint32_t IRAM_ATTR npl_os_hw_enter_critical(void)
{
  hw_critical_state_status++;
  esp_os_enter_critical_safe(&ble_port_lock);
  return 0;
}

uint8_t IRAM_ATTR npl_os_hw_is_in_critical(void)
{
  return hw_critical_state_status;
}

void IRAM_ATTR npl_os_hw_exit_critical(uint32_t ctx)
{
  (void)ctx;
  hw_critical_state_status--;
  esp_os_exit_critical_safe(&ble_port_lock);
}

uint32_t IRAM_ATTR npl_os_get_time_forever(void)
{
  return portMAX_DELAY;
}

static const struct npl_funcs_t npl_funcs_ro = {
  .p_ble_npl_os_started = npl_os_os_started,
  .p_ble_npl_get_current_task_id = npl_os_get_current_task_id,
  .p_ble_npl_eventq_init = npl_os_eventq_init,
  .p_ble_npl_eventq_deinit = npl_os_eventq_deinit,
  .p_ble_npl_eventq_get = npl_os_eventq_get,
  .p_ble_npl_eventq_put = npl_os_eventq_put,
  .p_ble_npl_eventq_put_to_front = npl_os_eventq_put_to_front,
  .p_ble_npl_eventq_remove = npl_os_eventq_remove,
  .p_ble_npl_event_run = npl_os_event_run,
  .p_ble_npl_eventq_is_empty = npl_os_eventq_is_empty,
  .p_ble_npl_event_init = npl_os_event_init,
  .p_ble_npl_event_deinit = npl_os_event_deinit,
  .p_ble_npl_event_reset = npl_os_event_reset,
  .p_ble_npl_event_is_queued = npl_os_event_is_queued,
  .p_ble_npl_event_get_arg = npl_os_event_get_arg,
  .p_ble_npl_event_set_arg = npl_os_event_set_arg,
  .p_ble_npl_mutex_init = npl_os_mutex_init,
  .p_ble_npl_mutex_deinit = npl_os_mutex_deinit,
  .p_ble_npl_mutex_pend = npl_os_mutex_pend,
  .p_ble_npl_mutex_release = npl_os_mutex_release,
  .p_ble_npl_sem_init = npl_os_sem_init,
  .p_ble_npl_sem_deinit = npl_os_sem_deinit,
  .p_ble_npl_sem_pend = npl_os_sem_pend,
  .p_ble_npl_sem_release = npl_os_sem_release,
  .p_ble_npl_sem_get_count = npl_os_sem_get_count,
  .p_ble_npl_callout_init = npl_os_callout_init,
  .p_ble_npl_callout_reset = npl_os_callout_reset,
  .p_ble_npl_callout_stop = npl_os_callout_stop,
  .p_ble_npl_callout_deinit = npl_os_callout_deinit,
  .p_ble_npl_callout_mem_reset = npl_os_callout_mem_reset,
  .p_ble_npl_callout_is_active = npl_os_callout_is_active,
  .p_ble_npl_callout_get_ticks = npl_os_callout_get_ticks,
  .p_ble_npl_callout_remaining_ticks = npl_os_callout_remaining_ticks,
  .p_ble_npl_callout_set_arg = npl_os_callout_set_arg,
  .p_ble_npl_time_get = npl_os_time_get,
  .p_ble_npl_time_ms_to_ticks = npl_os_time_ms_to_ticks,
  .p_ble_npl_time_ticks_to_ms = npl_os_time_ticks_to_ms,
  .p_ble_npl_time_ms_to_ticks32 = npl_os_time_ms_to_ticks32,
  .p_ble_npl_time_ticks_to_ms32 = npl_os_time_ticks_to_ms32,
  .p_ble_npl_time_delay = npl_os_time_delay,
#if NIMBLE_CFG_CONTROLLER || CONFIG_NIMBLE_CONTROLLER_MODE
  .p_ble_npl_hw_set_isr = NULL,
#endif
  .p_ble_npl_hw_enter_critical = npl_os_hw_enter_critical,
  .p_ble_npl_hw_exit_critical = npl_os_hw_exit_critical,
  .p_ble_npl_get_time_forever = npl_os_get_time_forever,
  .p_ble_npl_hw_is_in_critical = npl_os_hw_is_in_critical,
};

struct npl_funcs_t *npl_funcs;

struct npl_funcs_t *npl_os_funcs_get(void)
{
  return npl_funcs;
}

void npl_os_funcs_init(void)
{
  npl_funcs = (struct npl_funcs_t *)bt_osi_mem_malloc_internal(sizeof(*npl_funcs));
  BLE_LL_ASSERT(npl_funcs);
  memcpy(npl_funcs, &npl_funcs_ro, sizeof(*npl_funcs));
}

void npl_os_funcs_deinit(void)
{
  if (npl_funcs)
    {
      bt_osi_mem_free_internal(npl_funcs);
    }

  npl_funcs = NULL;
}

int npl_os_set_controller_npl_info(ble_npl_count_info_t *ctrl_npl_info)
{
  if (!ctrl_npl_info)
    {
      return -1;
    }

  memcpy(&g_ctrl_npl_info, ctrl_npl_info, sizeof(g_ctrl_npl_info));
  return 0;
}

int npl_os_mempool_init(void)
{
  ble_os_total_event_cnt = g_ctrl_npl_info.evt_count + BLE_HOST_EV_COUNT;
  if (ble_os_total_event_cnt == 0)
    {
      ble_os_total_event_cnt = 1;
    }

  return 0;
}

void npl_os_mempool_deinit(void)
{
}
