/****************************************************************************
 * nxtmpdir/esp-hal-3rdparty/nuttx/include/platform/os.h
 *
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

#ifndef __NXTMPDIR_ESP_HAL_3RDPARTY_NUTTX_INCLUDE_PLATFORM_OS_H
#define __NXTMPDIR_ESP_HAL_3RDPARTY_NUTTX_INCLUDE_PLATFORM_OS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "sdkconfig.h"
#include <stddef.h>
#include <stdint.h>
#include <nuttx/clock.h>
#include <nuttx/sched.h>
#include <nuttx/init.h>
#include <sched/sched.h>

#include "spinlock.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_err.h"

#ifdef __NuttX__
#  ifdef CONFIG_IDF_TARGET_ESP32
#    include "esp32_rt_timer.h"
#    define esp_timer_get_time rt_timer_time_us
#  elif defined(CONFIG_IDF_TARGET_ESP32S3)
#    include "esp32s3_rt_timer.h"
#    define esp_timer_get_time esp32s3_rt_timer_time_us
#  elif defined(CONFIG_IDF_TARGET_ESP32S2)
#    include "esp32s2_rt_timer.h"
#    define esp_timer_get_time rt_timer_time_us
#  else
#    include "esp_hr_timer.h"
#    define esp_timer_get_time esp_hr_timer_time_us
#    define esp_timer_create esp_hr_timer_create
#    define esp_timer_start_once esp_hr_timer_start_once
#    define esp_timer_start_periodic esp_hr_timer_start_periodic
#    define esp_timer_stop esp_hr_timer_stop
#    define esp_timer_delete esp_hr_timer_delete
#    define esp_timer_private_set esp_hr_timer_set
#    define esp_timer_private_lock esp_hr_timer_lock
#    define esp_timer_private_unlock esp_hr_timer_unlock
#  endif
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* In theory, OS_SPINLOCK should only be defined in a multi-core
 * environment, because critical-section-related functions there take a lock
 * as a parameter.  In practice, since the Xtensa FreeRTOS port layer is the
 * same for the ESP32, S3, and S2, the latter also requires a parameter in
 * its critical-section-related functions.
 */

#if (!CONFIG_FREERTOS_UNICORE || CONFIG_IDF_TARGET_ARCH_XTENSA) && \
    !NON_OS_BUILD
/* This macro also helps users switching between spinlock declarations/
 * definitions for multi-/single core environments if the macros below
 * aren't sufficient.
 */

#  define OS_SPINLOCK 1
#else
#  define OS_SPINLOCK 0
#endif

#define OS_PORT_YIELD_FROM_ISR()
#define OS_PORT_MAX_DELAY           0xfffffffful
#define OS_PORT_TICKS_TO_MS(ticks)  MSEC2TICK(ticks)
#define OS_BASE_TYPE                int
#define OS_FALSE                    FALSE
#define OS_TRUE                     TRUE
#define traceISR_ENTER(param1)
#define traceISR_EXIT(param1)
#define os_task_switch_is_pended(_cpu_) (false)
#define OS_PORT_NUM_PROCESSORS      CONFIG_FREERTOS_NUMBER_OF_CORES
#define OS_PORT_GET_CORE_ID()       this_cpu()
#define OS_PORT_SUSPEND_SCHEDULER() sched_lock()
#define OS_TASK_DELAY(ticks)        esp_os_task_delay_adapter(ticks)
#define OS_SCHEDULER_RUNNING()      OSINIT_OS_READY()
#define OS_TICK_PERIOD_MS           (CONFIG_USEC_PER_TICK / 1000)
#define OS_IN_ISR()                 up_interrupt_context()
#define OS_TASK_PRIO_MAX            (SCHED_PRIORITY_MAX)
#define OS_TASK_PRIO_MIN            (SCHED_PRIORITY_MIN)
#define OS_TASK_GET_CORE_ID(task)   esp_os_task_get_core_id(task)
#define OS_SPINLOCK_TYPE            rspinlock_t
#define OS_SPINLOCK_INITIALIZER     RSPINLOCK_INITIALIZER
#define OS_SPINLOCK_INIT(lock)      esp_os_spinlock_initialize(lock)

#if OS_SPINLOCK == 1
#  define OS_ENTER_CRITICAL_WITH_LOCK(lock)      nuttx_enter_critical(lock)
#  define OS_EXIT_CRITICAL_WITH_LOCK(lock)       nuttx_exit_critical(lock)
#  define OS_ENTER_CRITICAL_WITH_LOCK_ISR(lock)  nuttx_enter_critical(lock)
#  define OS_EXIT_CRITICAL_WITH_LOCK_ISR(lock)   nuttx_exit_critical(lock)
#  define OS_ENTER_CRITICAL_WITH_LOCK_SAFE(lock) nuttx_enter_critical(lock)
#  define OS_EXIT_CRITICAL_WITH_LOCK_SAFE(lock)  nuttx_exit_critical(lock)
#else
#  define OS_ENTER_CRITICAL_NO_LOCK()            nuttx_enter_critical()
#  define OS_EXIT_CRITICAL_NO_LOCK()             nuttx_exit_critical()
#  define OS_ENTER_CRITICAL_NO_LOCK_ISR()        nuttx_enter_critical()
#  define OS_EXIT_CRITICAL_NO_LOCK_ISR()         nuttx_exit_critical()
#  define OS_ENTER_CRITICAL_NO_LOCK_SAFE()       nuttx_enter_critical()
#  define OS_EXIT_CRITICAL_NO_LOCK_SAFE()        nuttx_exit_critical()
#endif

/* FreeRTOS compatibility macros for NuttX. */

#define tskNO_AFFINITY \
  ((1 << CONFIG_FREERTOS_NUMBER_OF_CORES) - 1)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* FreeRTOS type compatibility for NuttX. */

typedef uint32_t UBaseType_t;
typedef int32_t BaseType_t;

struct mq_adpt
{
  struct file mq;       /* Message queue handle. */
  uint32_t msgsize;     /* Message size. */
  char name[16];        /* Message queue name. */
};

struct irq_adpt
{
  CODE void (*func)(FAR void *arg);  /* Interrupt callback function. */
  FAR void *arg;                     /* Interrupt private data. */
};

struct esp_os_intr_handle_t
{
  FAR intr_handle_t *intr_handle;
  int irq;
};

typedef struct esp_os_intr_handle_t esp_os_intr_handle_t;
typedef FAR struct mq_adpt *esp_os_queue_handle_t;
typedef uint32_t esp_os_tick_type_t;
typedef intr_handler_t esp_os_intr_handler_t;
typedef rmutex_t esp_os_recursive_mutex_t;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/* IRQ allocation functions */

esp_err_t esp_os_intr_free(intr_handle_t handle);

esp_err_t esp_os_intr_alloc(int source, int flags,
                             esp_os_intr_handler_t handler, FAR void *arg,
                             FAR intr_handle_t *ret_handle);

esp_err_t esp_os_intr_alloc_intrstatus(int source, int flags,
                                        uint32_t intrstatusreg,
                                        uint32_t intrstatusmask,
                                        esp_os_intr_handler_t handler,
                                        FAR void *arg,
                                        FAR intr_handle_t *ret_handle);

/* Queue functions */

#define DECLARE_QUEUE_IN_STRUCT(queue_name) \
  esp_os_queue_handle_t queue_name;

esp_os_queue_handle_t esp_os_queue_create_with_caps(size_t max_items,
                                                     size_t item_size,
                                                     uint32_t caps);

esp_err_t esp_os_queue_send(esp_os_queue_handle_t queue, FAR void *item,
                             uint32_t ticks);

esp_err_t esp_os_queue_send_from_isr(esp_os_queue_handle_t queue,
                                      FAR void *item, FAR void *hptw);

esp_err_t esp_os_queue_receive(esp_os_queue_handle_t queue, FAR void *item,
                                uint32_t ticks);

esp_err_t esp_os_queue_receive_from_isr(esp_os_queue_handle_t queue,
                                         FAR void *item, FAR void *hptw);

void esp_os_queue_delete_with_caps(esp_os_queue_handle_t queue);

void esp_os_queue_delete(esp_os_queue_handle_t queue);

/* Recursive mutex functions */

void esp_os_create_recursive_mutex(FAR esp_os_recursive_mutex_t *mutex);

void esp_os_lock_recursive_mutex(FAR esp_os_recursive_mutex_t *mutex);

void esp_os_unlock_recursive_mutex(FAR esp_os_recursive_mutex_t *mutex);

/* Scheduler control functions */

void esp_os_scheduler_disable(void);

void esp_os_scheduler_enable(void);

bool esp_os_scheduler_started(void);

#ifdef __cplusplus
}
#endif

/****************************************************************************
 * Public Data
 ****************************************************************************/

#if OS_SPINLOCK == 1
extern void nuttx_enter_critical(FAR rspinlock_t *lock);
extern void nuttx_exit_critical(FAR rspinlock_t *lock);
#else
extern void nuttx_enter_critical(void);
extern void nuttx_exit_critical(void);
#endif

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

/****************************************************************************
 * Name: esp_os_task_delay_adapter
 *
 * Description:
 *   Delay for a number of ticks.
 *
 * Input Parameters:
 *   ticks - Number of ticks to delay.
 *
 ****************************************************************************/

static inline void esp_os_task_delay_adapter(uint32_t ticks)
{
  if (OSINIT_OS_READY())
    {
      useconds_t us = TICK2USEC(ticks);
      nxsched_usleep(us);
    }
}

/****************************************************************************
 * Name: esp_os_task_get_core_id
 *
 * Description:
 *   Get the core ID of a task (FreeRTOS compatibility).
 *   In NuttX, we return tskNO_AFFINITY if the task can run on multiple
 *   cores, or the specific CPU ID if pinned to a single core.
 *
 * Input Parameters:
 *   task - Task handle (NULL for current task).
 *
 * Returned Value:
 *   Core ID or tskNO_AFFINITY.
 *
 ****************************************************************************/

static inline UBaseType_t esp_os_task_get_core_id(FAR void *task)
{
#ifdef CONFIG_SMP
  FAR struct tcb_s *tcb = task ? (FAR struct tcb_s *)task : this_task();
  cpu_set_t affinity = tcb->affinity;
  int count = 0;
  int cpu_id = 0;
  int i;

  for (i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++)
    {
      if (CPU_ISSET(i, &affinity))
        {
          count++;
          cpu_id = i;
        }
    }

  /* If pinned to a single core, return that core ID. */

  if (count == 1)
    {
      return cpu_id;
    }

  /* Otherwise, return tskNO_AFFINITY (task can run on any core). */

  return tskNO_AFFINITY;
#else
  /* Non-SMP builds: always return core 0 (the only core). */

  return 0;
#endif
}

/****************************************************************************
 * Name: esp_os_spinlock_initialize
 *
 * Description:
 *   Initialize a spinlock.
 *
 * Input Parameters:
 *   lock - Pointer to the spinlock.
 *
 ****************************************************************************/

static inline void
__attribute__((always_inline)) esp_os_spinlock_initialize(FAR rspinlock_t *
                                                           lock)
{
  DEBUGASSERT(lock != NULL);

  rspin_lock_init(lock);
}

#endif /* __NXTMPDIR_ESP_HAL_3RDPARTY_NUTTX_INCLUDE_PLATFORM_OS_H */
