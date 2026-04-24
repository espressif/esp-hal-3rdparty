/****************************************************************************
 * nxtmpdir/esp-hal-3rdparty/nuttx/src/esp_event.c
 *
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/* Minimal NuttX reimplementation of ESP-IDF's esp_event default loop.
 *
 * This file provides only the subset of the esp_event API actually called
 * by the NuttX glue for Wi-Fi (esp_event_post_wrapper), Ethernet (esp_eth
 * posting ETH_EVENT/ETHERNET_EVENT_* from its own link-check task), and
 * their respective NuttX-side handlers:
 *
 *   - esp_event_post
 *   - esp_event_handler_register
 *   - esp_event_handler_unregister
 *   - esp_evt_work_init
 *
 * Handlers are dispatched asynchronously in the low-priority work queue
 * (LPWORK). Events are FIFO-queued under a spinlock-protected sq_queue_t;
 * the handler registry is protected by a separate mutex.
 *
 * The WiFi-specific switch-based dispatcher (formerly in
 * arch/{risc-v,xtensa}/src/common/espressif/esp_wifi_event_handler.c) has
 * been moved out into esp_wifi_events.c, which registers itself against
 * WIFI_EVENT via esp_event_handler_register().
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <debug.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <nuttx/config.h>
#include <nuttx/kmalloc.h>
#include <nuttx/mutex.h>
#include <nuttx/queue.h>
#include <nuttx/spinlock.h>
#include <nuttx/wqueue.h>

#include "esp_err.h"
#include "esp_event.h"

#ifndef CONFIG_SCHED_LPWORK
#  error "CONFIG_SCHED_LPWORK must be defined"
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* Queued event record. The variable-length buf[] carries a copy of the
 * event_data supplied to esp_event_post() so the caller can release its
 * buffer immediately upon return.
 */

struct evt_adpt
{
  sq_entry_t entry;
  esp_event_base_t base;        /* Pointer identity; const char * from IDF */
  int32_t id;
  size_t size;
  uint8_t buf[0];
};

/* Registered event handler record. */

struct evt_handler
{
  sq_entry_t entry;
  esp_event_base_t base;        /* ESP_EVENT_ANY_BASE (NULL) matches any */
  int32_t id;                   /* ESP_EVENT_ANY_ID (-1) matches any    */
  esp_event_handler_t handler;
  void *arg;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct work_s g_evt_work;
static sq_queue_t g_evt_queue;
static sq_queue_t g_handler_queue;
static mutex_t g_handler_lock = NXMUTEX_INITIALIZER;
static spinlock_t g_evt_lock;
static bool g_evt_initialized;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: esp_evt_match
 *
 * Description:
 *   Returns true if the handler registration record matches the event
 *   (base, id) pair, honouring ESP_EVENT_ANY_BASE / ESP_EVENT_ANY_ID
 *   wildcards.
 *
 * Input Parameters:
 *   h    - Pointer to a registered handler record to test.
 *   base - Event base value of the incoming event.
 *   id   - Event ID value of the incoming event.
 *
 * Returned Value:
 *   true  if the handler matches the incoming event.
 *   false otherwise.
 *
 ****************************************************************************/

static bool esp_evt_match(const struct evt_handler *h,
                          esp_event_base_t base, int32_t id)
{
  if (h->base != ESP_EVENT_ANY_BASE && h->base != base)
    {
      return false;
    }

  if (h->id != ESP_EVENT_ANY_ID && h->id != id)
    {
      return false;
    }

  return true;
}

/****************************************************************************
 * Name: esp_evt_work_cb
 *
 * Description:
 *   Low-priority work-queue callback. Drains the pending event queue and
 *   dispatches every event to all registered handlers whose (base, id)
 *   filter matches.
 *
 * Input Parameters:
 *   arg - Unused callback argument.
 *
 * Returned Value:
 *   None.
 *
 ****************************************************************************/

static void esp_evt_work_cb(void *arg)
{
  UNUSED(arg);
  irqstate_t flags;
  struct evt_adpt *evt;
  struct evt_handler *h;

  while (1)
    {
      flags = spin_lock_irqsave(&g_evt_lock);
      evt = (struct evt_adpt *)sq_remfirst(&g_evt_queue);
      spin_unlock_irqrestore(&g_evt_lock, flags);

      if (evt == NULL)
        {
          break;
        }

      nxmutex_lock(&g_handler_lock);

      for (h = (struct evt_handler *)sq_peek(&g_handler_queue);
           h != NULL;
           h = (struct evt_handler *)sq_next(&h->entry))
        {
          if (esp_evt_match(h, evt->base, evt->id))
            {
              h->handler(h->arg, evt->base, evt->id,
                         evt->size ? (void *)evt->buf : NULL);
            }
        }

      nxmutex_unlock(&g_handler_lock);

      kmm_free(evt);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: esp_evt_work_init
 *
 * Description:
 *   Idempotently initialize the generic event queue and handler registry.
 *   Safe to call multiple times; subsequent calls are no-ops.
 *
 * Input Parameters:
 *   None.
 *
 * Returned Value:
 *   None.
 *
 ****************************************************************************/

void esp_evt_work_init(void)
{
  if (g_evt_initialized)
    {
      return;
    }

  sq_init(&g_evt_queue);
  sq_init(&g_handler_queue);
  g_evt_initialized = true;
}

/****************************************************************************
 * Name: esp_event_post
 *
 * Description:
 *   Post an event (base, id, data) to the default event loop. The event
 *   data buffer is copied; the caller may free/reuse it immediately.
 *
 * Input Parameters:
 *   event_base      - Event category (const char * pointer identity).
 *   event_id        - Event ID within the base.
 *   event_data      - Pointer to event data (may be NULL if size is 0).
 *   event_data_size - Size of event data in bytes.
 *   ticks_to_wait   - Unused on NuttX (no bounded producer queue).
 *
 * Returned Value:
 *   ESP_OK on success, ESP_ERR_NO_MEM on allocation failure.
 *
 ****************************************************************************/

esp_err_t esp_event_post(esp_event_base_t event_base, int32_t event_id,
                         void *event_data, size_t event_data_size,
                         TickType_t ticks_to_wait)
{
  size_t size;
  irqstate_t flags;
  struct evt_adpt *evt;

  UNUSED(ticks_to_wait);

  if (!g_evt_initialized)
    {
      esp_evt_work_init();
    }

  size = event_data_size + sizeof(struct evt_adpt);
  evt = kmm_malloc(size);
  if (evt == NULL)
    {
      return ESP_ERR_NO_MEM;
    }

  evt->base = event_base;
  evt->id = event_id;
  evt->size = event_data_size;
  if (event_data != NULL && event_data_size != 0)
    {
      memcpy(evt->buf, event_data, event_data_size);
    }

  flags = spin_lock_irqsave(&g_evt_lock);
  sq_addlast(&evt->entry, &g_evt_queue);
  spin_unlock_irqrestore(&g_evt_lock, flags);

  work_queue(LPWORK, &g_evt_work, esp_evt_work_cb, NULL, 0);

  return ESP_OK;
}

/****************************************************************************
 * Name: esp_event_handler_register
 *
 * Description:
 *   Register a handler to be invoked for events matching (event_base,
 *   event_id). ESP_EVENT_ANY_BASE / ESP_EVENT_ANY_ID act as wildcards.
 *
 * Input Parameters:
 *   event_base        - Event base to match, or ESP_EVENT_ANY_BASE.
 *   event_id          - Event ID to match, or ESP_EVENT_ANY_ID.
 *   event_handler     - Callback function to invoke for matching events.
 *   event_handler_arg - Opaque argument passed to event_handler.
 *
 * Returned Value:
 *   ESP_OK on success.
 *   ESP_ERR_INVALID_ARG if event_handler is NULL.
 *   ESP_ERR_NO_MEM if handler registration allocation fails.
 *
 ****************************************************************************/

esp_err_t esp_event_handler_register(esp_event_base_t event_base,
                                     int32_t event_id,
                                     esp_event_handler_t event_handler,
                                     void *event_handler_arg)
{
  struct evt_handler *h;

  if (event_handler == NULL)
    {
      return ESP_ERR_INVALID_ARG;
    }

  if (!g_evt_initialized)
    {
      esp_evt_work_init();
    }

  h = kmm_malloc(sizeof(*h));
  if (h == NULL)
    {
      return ESP_ERR_NO_MEM;
    }

  h->base = event_base;
  h->id = event_id;
  h->handler = event_handler;
  h->arg = event_handler_arg;

  nxmutex_lock(&g_handler_lock);
  sq_addlast(&h->entry, &g_handler_queue);
  nxmutex_unlock(&g_handler_lock);

  return ESP_OK;
}

/****************************************************************************
 * Name: esp_event_handler_unregister
 *
 * Description:
 *   Remove all handler registrations matching (event_base, event_id,
 *   event_handler). Silently succeeds if nothing matches.
 *
 * Input Parameters:
 *   event_base    - Event base used during registration.
 *   event_id      - Event ID used during registration.
 *   event_handler - Callback function pointer used during registration.
 *
 * Returned Value:
 *   ESP_OK on success (including no matching registration found).
 *   ESP_ERR_INVALID_ARG if event_handler is NULL.
 *
 ****************************************************************************/

esp_err_t esp_event_handler_unregister(esp_event_base_t event_base,
                                       int32_t event_id,
                                       esp_event_handler_t event_handler)
{
  struct evt_handler *h;
  struct evt_handler *next;

  if (event_handler == NULL)
    {
      return ESP_ERR_INVALID_ARG;
    }

  nxmutex_lock(&g_handler_lock);

  h = (struct evt_handler *)sq_peek(&g_handler_queue);
  while (h != NULL)
    {
      next = (struct evt_handler *)sq_next(&h->entry);

      if (h->base == event_base && h->id == event_id &&
          h->handler == event_handler)
        {
          sq_rem(&h->entry, &g_handler_queue);
          kmm_free(h);
        }

      h = next;
    }

  nxmutex_unlock(&g_handler_lock);

  return ESP_OK;
}
