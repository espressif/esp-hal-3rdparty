/****************************************************************************
 * nxtmpdir/esp-hal-3rdparty/nuttx/src/platform/os.c
 *
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <debug.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <clock/clock.h>

#include <nuttx/config.h>
#include <nuttx/mutex.h>
#include <nuttx/spinlock.h>
#include <nuttx/irq.h>
#include <nuttx/queue.h>
#include <nuttx/mqueue.h>
#include <nuttx/kmalloc.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <nuttx/sched.h>
#include <nuttx/mm/mm.h>

#include "sdkconfig.h"

#include "esp_irq.h"
#ifdef CONFIG_PM
#include "espressif/esp_pm.h"
#endif
#include "esp_hr_timer.h"

#include "esp_private/critical_section.h"
#include "esp_sleep.h"

#include "platform/os.h"

#if CONFIG_IDF_TARGET_ARCH_XTENSA
unsigned _xt_tick_divisor = 0;  /* cached number of cycles per tick */
#endif

/****************************************************************************
 * Name: esp_errno_to_esp_err
 *
 * Description:
 *   Convert a NuttX/posix errno-style return value to esp_err_t.
 *   See platform/os.h for full documentation.
 *
 ****************************************************************************/

esp_err_t esp_errno_to_esp_err(int errno_value)
{
  if (errno_value == 0)
    {
      return ESP_OK;
    }

  if (errno_value == -EINVAL)
    {
      return ESP_ERR_INVALID_ARG;
    }

  if (errno_value == -ENOMEM)
    {
      return ESP_ERR_NO_MEM;
    }

  if (errno_value == -EALREADY || errno_value == -EBUSY)
    {
      return ESP_ERR_INVALID_STATE;
    }

  return ESP_FAIL;
}

#ifdef CONFIG_IDF_TARGET_ARCH_RISCV
#  include "esp_timer.h"

/****************************************************************************
 * RISC-V only: esp_timer API adapter layer
 *
 * Adapts esp_hr_timer (NuttX) to the esp_timer (IDF) API: argument types
 * and return codes (int/errno vs esp_err_t). Temporary until a full
 * esp_timer replacement is available for RISC-V.
 ****************************************************************************/

uint64_t esp_timer_get_time(void)
{
  return esp_hr_timer_time_us();
}

esp_err_t esp_timer_create(const esp_timer_create_args_t *create_args,
                           esp_timer_handle_t *out_handle)
{
  struct esp_hr_timer_args_s args;

  args.callback = create_args->callback;
  args.arg = create_args->arg;
  args.name = create_args->name;
  args.skip_unhandled_events = create_args->skip_unhandled_events;

  struct esp_hr_timer_s *handle = NULL;
  int ret = esp_hr_timer_create(&args, &handle);

  if (ret != 0)
    {
      return esp_errno_to_esp_err(ret);
    }

  *out_handle = (esp_timer_handle_t)handle;
  return ESP_OK;
}

esp_err_t esp_timer_start_once(esp_timer_handle_t timer, uint64_t timeout_us)
{
  int ret = esp_hr_timer_start_once((struct esp_hr_timer_s *)timer,
                                    timeout_us);
  return esp_errno_to_esp_err(ret);
}

esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period_us)
{
  int ret = esp_hr_timer_start_periodic((struct esp_hr_timer_s *)timer,
                                        period_us);
  return esp_errno_to_esp_err(ret);
}

esp_err_t esp_timer_stop(esp_timer_handle_t timer)
{
  int ret = esp_hr_timer_stop((struct esp_hr_timer_s *)timer);
  return esp_errno_to_esp_err(ret);
}

esp_err_t esp_timer_delete(esp_timer_handle_t timer)
{
  int ret = esp_hr_timer_delete((struct esp_hr_timer_s *)timer);
  return esp_errno_to_esp_err(ret);
}

void esp_timer_private_set(uint64_t new_us)
{
  esp_hr_timer_set(new_us);
}

void esp_timer_private_lock(void)
{
  esp_hr_timer_lock();
}

void esp_timer_private_unlock(void)
{
  esp_hr_timer_unlock();
}

#endif /* CONFIG_IDF_TARGET_ARCH_RISCV */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define OS_PORT_MAX_DELAY 0xfffffffful

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef struct shared_vector_desc_t shared_vector_desc_t;
typedef struct vector_desc_t vector_desc_t;

struct shared_vector_desc_t
{
  int disabled : 1;
  int source : 16;
  FAR volatile uint32_t *statusreg;
  uint32_t statusmask;
  intr_handler_t isr;
  FAR void *arg;
  FAR shared_vector_desc_t *next;
};

#define VECDESC_FL_RESERVED     (1 << 0)
#define VECDESC_FL_INIRAM       (1 << 1)
#define VECDESC_FL_SHARED       (1 << 2)
#define VECDESC_FL_NONSHARED    (1 << 3)
#define VECDESC_FL_TYPE_MASK    (0xf)

#if SOC_CPU_HAS_FLEXIBLE_INTC
/* On targets that have configurable interrupts levels, store the
 * assigned level in the flags.
 */

#  define VECDESC_FL_LEVEL_SHIFT  (8)

/* Allocate 4 bits in the flag. */

#  define VECDESC_FL_LEVEL_MASK   (0xf)

/* Help to extract the level from flags. */

#  define VECDESC_FL_LEVEL(flags) \
    (((flags) >> VECDESC_FL_LEVEL_SHIFT) & VECDESC_FL_LEVEL_MASK)
#endif

/* Pack using bitfields for better memory use. */

struct vector_desc_t
{
  int flags : 16;                          /* OR of VECDESC_FL_* defines. */
  unsigned int cpu : 1;
  unsigned int intno : 5;
  int source : 16;                         /* Interrupt mux flags, used when
                                            * not shared.
                                            */
  FAR shared_vector_desc_t *shared_vec_info; /* Used when
                                              * VECDESC_FL_SHARED.
                                              */
  FAR vector_desc_t *next;
};

/* Interrupt handler associated data structure. */

struct intr_handle_data_t
{
  FAR vector_desc_t *vector_desc;
  FAR shared_vector_desc_t *shared_vector_desc;
};

typedef struct intr_handle_data_t intr_handle_data_t;

typedef struct non_shared_isr_arg_t non_shared_isr_arg_t;

struct non_shared_isr_arg_t
{
  intr_handler_t isr;
  FAR void *isr_arg;
  int source;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Linked list of vector descriptions, sorted by cpu.intno value. */

static FAR vector_desc_t *vector_desc_head = NULL;

/* Critical section state per CPU. */

static uint8_t g_int_flags_count[OS_PORT_NUM_PROCESSORS];
static irqstate_t g_int_flags[OS_PORT_NUM_PROCESSORS];

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static esp_err_t esp_os_queue_send_generic(esp_os_queue_handle_t queue,
                                           FAR void *item,
                                           uint32_t ticks,
                                           int prio);

void esp_os_include_impl(void);
extern uint32_t up_get_idletime(void);
extern void vApplicationSleep(TickType_t xExpectedIdleTime);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: esp_os_queue_send_generic
 *
 * Description:
 *   Generic function to send a message to a queue.
 *
 * Input Parameters:
 *   queue - Queue handle.
 *   item  - Pointer to the item to be sent.
 *   ticks - Ticks to wait.
 *   prio  - Priority of the message.
 *
 * Returned Value:
 *   ESP_OK on success; ESP_FAIL on failure.
 *
 ****************************************************************************/

static esp_err_t esp_os_queue_send_generic(esp_os_queue_handle_t queue,
                                           FAR void *item,
                                           uint32_t ticks,
                                           int prio)
{
  int ret;
  struct timespec timeout;
  FAR struct mq_adpt *mq_adpt = (FAR struct mq_adpt *)queue;

  if (ticks == OS_PORT_MAX_DELAY || ticks == 0)
    {
      ret = file_mq_send(&mq_adpt->mq, (FAR const char *)item,
                         mq_adpt->msgsize, prio);
      if (ret < 0)
        {
          _err("Failed to send message to mqueue error=%d\n", ret);
        }
    }
  else
    {
      ret = clock_gettime(CLOCK_REALTIME, &timeout);
      if (ret < 0)
        {
          _err("Failed to get time\n");
          return false;
        }

      if (ticks)
        {
          struct timespec ts;

          clock_ticks2time(&ts, ticks);
          clock_timespec_add(&timeout, &ts, &timeout);
        }

      ret = file_mq_timedsend(&mq_adpt->mq, (FAR const char *)item,
                              mq_adpt->msgsize, prio, &timeout);
      if (ret < 0)
        {
          _err("Failed to timedsend message to mqueue error=%d\n", ret);
        }
    }

  return ret == 0 ? ESP_OK : ESP_FAIL;
}

/****************************************************************************
 * Name: esp_os_startup_start_app_other_cores
 *
 * Description:
 *   No-op function to replace the default implementation.
 *
 ****************************************************************************/

void IRAM_ATTR esp_os_startup_start_app_other_cores(void)
{
  /* No-op: This function is used to replace the default implementation
   * of esp_startup_start_app_other_cores.
   */
}

/****************************************************************************
 * Name: esp_os_queue_receive_generic
 *
 * Description:
 *   Generic function to receive a message from a queue.
 *
 * Input Parameters:
 *   queue - Queue handle.
 *   item  - Pointer to the buffer to receive the item.
 *   ticks - Ticks to wait.
 *
 * Returned Value:
 *   ESP_OK on success; ESP_FAIL on failure.
 *
 ****************************************************************************/

static esp_err_t esp_os_queue_receive_generic(esp_os_queue_handle_t queue,
                                              FAR void *item,
                                              uint32_t ticks)
{
  ssize_t ret;
  struct timespec timeout;
  unsigned int prio;
  FAR struct mq_adpt *mq_adpt = (FAR struct mq_adpt *)queue;

  if (ticks == OS_PORT_MAX_DELAY || up_interrupt_context())
    {
      ret = file_mq_receive(&mq_adpt->mq, (FAR char *)item,
                            mq_adpt->msgsize, &prio);
      if (ret < 0 && ret != -EAGAIN)
        {
          _err("Failed to receive from mqueue error=%d\n", ret);
        }
    }
  else
    {
      ret = clock_gettime(CLOCK_REALTIME, &timeout);
      if (ret < 0)
        {
          _err("Failed to get time\n");
          return false;
        }

      if (ticks)
        {
          struct timespec ts;

          clock_ticks2time(&ts, ticks);
          clock_timespec_add(&timeout, &ts, &timeout);
        }

      ret = file_mq_timedreceive(&mq_adpt->mq, (FAR char *)item,
                                 mq_adpt->msgsize, &prio, &timeout);
      if (ret < 0 && ret != -EAGAIN)
        {
          _err("Failed to timedreceive from mqueue error=%d\n", ret);
        }
    }

  return ret >= 0 ? ESP_OK : ESP_FAIL;
}

/****************************************************************************
 * Name: isr_adapter_func
 *
 * Description:
 *   Interrupt service routine adapter function.
 *
 * Input Parameters:
 *   irq     - IRQ number.
 *   context - Context (unused).
 *   arg     - User argument.
 *
 * Returned Value:
 *   Zero (OK).
 *
 ****************************************************************************/

static int isr_adapter_func(int irq, FAR void *context, FAR void *arg)
{
  FAR struct intr_adapter_to_nuttx *isr_adapter_args =
    (FAR struct intr_adapter_to_nuttx *)arg;

  isr_adapter_args->handler(isr_adapter_args->arg);

  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: esp_os_intr_free
 *
 * Description:
 *   Free an interrupt handler.
 *
 * Input Parameters:
 *   handle - Interrupt handle.
 *
 * Returned Value:
 *   ESP_OK on success.
 *
 ****************************************************************************/

esp_err_t esp_os_intr_free(intr_handle_t handle)
{
  FAR esp_os_intr_handle_t *intr_handle =
    (FAR esp_os_intr_handle_t *)handle;
  int irq = intr_handle->irq;
  int cpuint = esp_get_cpuint(this_cpu(), irq);

  esp_teardown_irq(ESP_IRQ2SOURCE(irq), cpuint);

  return ESP_OK;
}

/****************************************************************************
 * Name: esp_os_intr_alloc
 *
 * Description:
 *   Allocate an interrupt handler.
 *
 * Input Parameters:
 *   source     - Interrupt source.
 *   flags      - Interrupt flags.
 *   handler    - Interrupt handler function.
 *   arg        - User argument.
 *   ret_handle - Pointer to receive the interrupt handle.
 *
 * Returned Value:
 *   ESP_OK on success; error code on failure.
 *
 ****************************************************************************/

esp_err_t esp_os_intr_alloc(int source, int flags,
                             esp_os_intr_handler_t handler, FAR void *arg,
                             FAR intr_handle_t *ret_handle)
{
  return esp_os_intr_alloc_intrstatus(source, flags, 0, 0, handler, arg,
                                      ret_handle);
}

/****************************************************************************
 * Name: esp_os_intr_alloc_intrstatus
 *
 * Description:
 *   Allocate an interrupt handler with interrupt status register.
 *
 * Input Parameters:
 *   source         - Interrupt source.
 *   flags          - Interrupt flags.
 *   intrstatusreg  - Interrupt status register.
 *   intrstatusmask - Interrupt status mask.
 *   handler        - Interrupt handler function.
 *   arg            - User argument.
 *   ret_handle     - Pointer to receive the interrupt handle.
 *
 * Returned Value:
 *   ESP_OK on success; error code on failure.
 *
 ****************************************************************************/

esp_err_t esp_os_intr_alloc_intrstatus(int source, int flags,
                                        uint32_t intrstatusreg,
                                        uint32_t intrstatusmask,
                                        esp_os_intr_handler_t handler,
                                        FAR void *arg,
                                        FAR intr_handle_t *ret_handle)
{
  FAR struct intr_adapter_to_nuttx *isr_adapter_args;
  int ret;
  int irq = ESP_SOURCE2IRQ(source);
  int cpuint;

  isr_adapter_args = kmm_calloc(1, sizeof(struct intr_adapter_to_nuttx));
  if (isr_adapter_args == NULL)
    {
      irqerr("Failed to kmm_calloc\n");
      return ESP_ERR_NO_MEM;
    }

  isr_adapter_args->handler = handler;
  isr_adapter_args->arg     = arg;

  cpuint = esp_setup_irq_with_flags_intrstatus(source, flags, intrstatusreg,
                                               intrstatusmask,
                                               isr_adapter_func,
                                               isr_adapter_args);

  if (cpuint < 0)
    {
      irqerr("Failed to setup interrupt\n");
      return ESP_ERR_NOT_FOUND;
    }

  *ret_handle = esp_get_handle(this_cpu(), irq);

  if (*ret_handle == NULL)
    {
      irqerr("Failed to get handle\n");
      return ESP_ERR_NOT_FOUND;
    }

  up_enable_irq(irq);

  return ESP_OK;
}

/****************************************************************************
 * Name: esp_os_queue_create_with_caps
 *
 * Description:
 *   Create a queue with specific memory capabilities.
 *
 * Input Parameters:
 *   max_items - Maximum number of items in the queue.
 *   item_size - Size of each item.
 *   caps      - Memory capabilities (unused).
 *
 * Returned Value:
 *   Queue handle on success; NULL on failure.
 *
 ****************************************************************************/

esp_os_queue_handle_t esp_os_queue_create_with_caps(size_t max_items,
                                                     size_t item_size,
                                                     uint32_t caps)
{
  struct mq_attr attr;
  FAR struct mq_adpt *mq_adpt;
  int ret;

  mq_adpt = kmm_malloc(sizeof(struct mq_adpt));
  if (!mq_adpt)
    {
      wlerr("Failed to kmm_malloc\n");
      return NULL;
    }

  snprintf(mq_adpt->name, sizeof(mq_adpt->name),
           "/tmp/%p", mq_adpt);

  attr.mq_maxmsg  = max_items;
  attr.mq_msgsize = item_size;
  attr.mq_curmsgs = 0;
  attr.mq_flags   = 0;

  ret = file_mq_open(&mq_adpt->mq, mq_adpt->name, O_RDWR | O_CREAT, 0644,
                     &attr);
  if (ret < 0)
    {
      wlerr("Failed to create mqueue\n");
      kmm_free(mq_adpt);
      return NULL;
    }

  mq_adpt->msgsize = item_size;

  return (esp_os_queue_handle_t)mq_adpt;
}

/****************************************************************************
 * Name: esp_os_queue_send
 *
 * Description:
 *   Send a message to a queue.
 *
 * Input Parameters:
 *   queue - Queue handle.
 *   item  - Pointer to the item to be sent.
 *   ticks - Ticks to wait.
 *
 * Returned Value:
 *   ESP_OK on success; ESP_FAIL on failure.
 *
 ****************************************************************************/

esp_err_t esp_os_queue_send(esp_os_queue_handle_t queue, FAR void *item,
                            uint32_t ticks)
{
  return esp_os_queue_send_generic(queue, item, ticks, 0);
}

/****************************************************************************
 * Name: esp_os_queue_send_from_isr
 *
 * Description:
 *   Send a message to a queue from an interrupt context.
 *
 * Input Parameters:
 *   queue - Queue handle.
 *   item  - Pointer to the item to be sent.
 *   hptw  - Pointer to higher priority task woken flag.
 *
 * Returned Value:
 *   ESP_OK on success; ESP_FAIL on failure.
 *
 ****************************************************************************/

esp_err_t esp_os_queue_send_from_isr(esp_os_queue_handle_t queue,
                                     FAR void *item, FAR void *hptw)
{
  *(FAR int *)hptw = 0;

  return esp_os_queue_send_generic(queue, item, 0, 0);
}

/****************************************************************************
 * Name: esp_os_queue_receive
 *
 * Description:
 *   Receive a message from a queue.
 *
 * Input Parameters:
 *   queue - Queue handle.
 *   item  - Pointer to the buffer to receive the item.
 *   ticks - Ticks to wait.
 *
 * Returned Value:
 *   ESP_OK on success; ESP_FAIL on failure.
 *
 ****************************************************************************/

esp_err_t esp_os_queue_receive(esp_os_queue_handle_t queue, FAR void *item,
                               uint32_t ticks)
{
  return esp_os_queue_receive_generic(queue, item, ticks);
}

/****************************************************************************
 * Name: esp_os_queue_receive_from_isr
 *
 * Description:
 *   Receive a message from a queue from an interrupt context.
 *
 * Input Parameters:
 *   queue - Queue handle.
 *   item  - Pointer to the buffer to receive the item.
 *   hptw  - Pointer to higher priority task woken flag.
 *
 * Returned Value:
 *   ESP_OK on success; ESP_FAIL on failure.
 *
 ****************************************************************************/

esp_err_t esp_os_queue_receive_from_isr(esp_os_queue_handle_t queue,
                                        FAR void *item, FAR void *hptw)
{
  esp_err_t ret;
  int flags;

  flags = file_fcntl(&queue->mq, F_GETFL);
  if ((flags & O_NONBLOCK) == 0)
    {
      if (file_fcntl(&queue->mq, F_SETFL, flags | O_NONBLOCK) == -1)
        {
          _err("Failed to set nonblock flag\n");
          return ESP_FAIL;
        }
    }

  *(FAR int *)hptw = 0;

  ret = esp_os_queue_receive_generic(queue, item, 0);

  /* Restore the original flags. */

  flags = file_fcntl(&queue->mq, F_GETFL);
  if (file_fcntl(&queue->mq, F_SETFL, flags & ~O_NONBLOCK) == -1)
    {
      _err("Failed to clear nonblock flag\n");
      return ESP_FAIL;
    }

  return ret;
}

/****************************************************************************
 * Name: esp_os_queue_delete_with_caps
 *
 * Description:
 *   Delete a queue.
 *
 * Input Parameters:
 *   queue - Queue handle.
 *
 ****************************************************************************/

void esp_os_queue_delete_with_caps(esp_os_queue_handle_t queue)
{
  FAR struct mq_adpt *mq_adpt = (FAR struct mq_adpt *)queue;

  ASSERT(file_mq_close(&mq_adpt->mq) == OK);
  ASSERT(file_mq_unlink(mq_adpt->name) == OK);
  kmm_free(mq_adpt);
}

/****************************************************************************
 * Name: esp_os_queue_delete
 *
 * Description:
 *   Delete a queue.
 *
 * Input Parameters:
 *   queue - Queue handle.
 *
 ****************************************************************************/

void esp_os_queue_delete(esp_os_queue_handle_t queue)
{
  esp_os_queue_delete_with_caps(queue);
}

/****************************************************************************
 * Name: esp_os_include_impl
 *
 * Description:
 *   No-op function to force linker inclusion.
 *
 ****************************************************************************/

void esp_os_include_impl(void)
{
  /* No-op: This function is used to force the linker to include
   * this file.
   */
}

/****************************************************************************
 * Name: esp_os_create_recursive_mutex
 *
 * Description:
 *   Initialize a recursive mutex.
 *
 * Input Parameters:
 *   mutex - Pointer to the recursive mutex to initialize.
 *
 ****************************************************************************/

void esp_os_create_recursive_mutex(FAR esp_os_recursive_mutex_t *mutex)
{
  nxrmutex_init(mutex);
}

/****************************************************************************
 * Name: esp_os_lock_recursive_mutex
 *
 * Description:
 *   Lock a recursive mutex.
 *
 * Input Parameters:
 *   mutex - Pointer to the recursive mutex to lock.
 *
 ****************************************************************************/

void esp_os_lock_recursive_mutex(FAR esp_os_recursive_mutex_t *mutex)
{
  nxrmutex_lock(mutex);
}

/****************************************************************************
 * Name: esp_os_unlock_recursive_mutex
 *
 * Description:
 *   Unlock a recursive mutex.
 *
 * Input Parameters:
 *   mutex - Pointer to the recursive mutex to unlock.
 *
 ****************************************************************************/

void esp_os_unlock_recursive_mutex(FAR esp_os_recursive_mutex_t *mutex)
{
  nxrmutex_unlock(mutex);
}

/****************************************************************************
 * Name: esp_os_scheduler_disable
 *
 * Description:
 *   Disable the task scheduler on the current CPU.
 *
 ****************************************************************************/

void esp_os_scheduler_disable(void)
{
  sched_lock();
}

/****************************************************************************
 * Name: esp_os_scheduler_enable
 *
 * Description:
 *   Re-enable the task scheduler on the current CPU.
 *
 ****************************************************************************/

void esp_os_scheduler_enable(void)
{
  sched_unlock();
}

/****************************************************************************
 * Name: esp_os_scheduler_started
 *
 * Description:
 *   Check if the OS scheduler has been started.
 *
 * Returned Value:
 *   true if the scheduler is running; false otherwise.
 *
 ****************************************************************************/

bool IRAM_ATTR esp_os_scheduler_started(void)
{
  return OSINIT_OS_READY();
}

/****************************************************************************
 * Name: esp_os_application_sleep
 *
 * Description:
 *   Convert NuttX idle time (microseconds) into OS ticks and forward it to
 *   vApplicationSleep(), which expects the idle duration in tick units.
 *
 ****************************************************************************/

void esp_os_application_sleep(void)
{
#ifdef CONFIG_PM
  uint32_t idle_us;
  TickType_t idle_ticks;

  /* The device can sleep for a maximum of CONFIG_PM_ALARM_SEC seconds
   * plus CONFIG_PM_ALARM_NSEC nanoseconds. This doesn't mean that this is
   * the actual time that the device will sleep. `vApplicationSleep` also
   * takes into account the time may wake up early due to an existing timer
   * alarm.
   */

  idle_us = CONFIG_PM_ALARM_SEC * 1000000 +
            CONFIG_PM_ALARM_NSEC / 1000000;
  idle_ticks = (TickType_t)(idle_us / OS_TICK_PERIOD_US);

  if (idle_ticks > 0)
    {
      vApplicationSleep(idle_ticks);
    }
#endif
}

/****************************************************************************
 * Name: nuttx_enter_critical
 *
 * Description:
 *   Enter a critical section.
 *
 * Input Parameters:
 *   lock - Spinlock (if OS_SPINLOCK == 1).
 *
 ****************************************************************************/

nooptimiziation_function
#if OS_SPINLOCK == 1
void nuttx_enter_critical(FAR rspinlock_t *lock)
#else
void nuttx_enter_critical(void)
#endif
{
  irqstate_t flags;
  int cpu = this_cpu();

  if (g_int_flags_count[cpu] == 0)
    {
      flags = up_irq_save();

      /* First time acquiring this lock. */

      g_int_flags[cpu] = flags;
    }

#if OS_SPINLOCK == 1
#  ifdef CONFIG_SPINLOCK
  rspin_lock(lock);
#  endif
#endif

  g_int_flags_count[cpu]++;
}

/****************************************************************************
 * Name: nuttx_exit_critical
 *
 * Description:
 *   Exit a critical section.
 *
 * Input Parameters:
 *   lock - Spinlock (if OS_SPINLOCK == 1).
 *
 ****************************************************************************/

nooptimiziation_function
#if OS_SPINLOCK == 1
void nuttx_exit_critical(FAR rspinlock_t *lock)
#else
void nuttx_exit_critical(void)
#endif
{
  int cpu = this_cpu();

  g_int_flags_count[cpu]--;

#if OS_SPINLOCK == 1
#  ifdef CONFIG_SPINLOCK
  rspin_unlock(lock);
#  endif
#endif

  if (g_int_flags_count[cpu] == 0)
    {
      up_irq_restore(g_int_flags[cpu]);
    }
}

/****************************************************************************
 * Task Management Implementation
 *
 * Per-task notification mechanism: each task created through
 * esp_os_create_task_pinned_to_core gets its own semaphore entry in a
 * dynamically allocated singly-linked list keyed by TID.  This allows
 * esp_os_task_notify_take() and esp_os_task_notify_give_from_isr() to
 * target the correct task without sharing a single global semaphore and
 * without an arbitrary upper bound on the number of concurrent tasks.
 *
 * The list head is protected with irqsave/restore so it can be safely
 * traversed from both task and ISR contexts.  Allocation (kmm_malloc) and
 * semaphore initialisation always happen outside the critical section.
 *
 ****************************************************************************/

struct task_notify_entry_s
{
  pid_t                          tid;  /* Owner task TID                 */
  sem_t                          sem;  /* Per-task counting semaphore    */
  FAR struct task_notify_entry_s *next; /* Next entry in list            */
};

/* Singly-linked list of active entries, protected by irqsave/restore. */

static FAR struct task_notify_entry_s *g_task_notify_head;

/****************************************************************************
 * Name: task_notify_find_locked
 *
 * Description:
 *   Return the list entry for `tid`, or NULL if not found.
 *   Caller MUST hold interrupts disabled (up_irq_save).
 *
 ****************************************************************************/

static FAR struct task_notify_entry_s *
task_notify_find_locked(pid_t tid)
{
  FAR struct task_notify_entry_s *entry;

  for (entry = g_task_notify_head; entry != NULL; entry = entry->next)
    {
      if (entry->tid == tid)
        {
          return entry;
        }
    }

  return NULL;
}

/****************************************************************************
 * Name: task_notify_register
 *
 * Description:
 *   Allocate a new list entry for `tid` and initialise its semaphore.
 *   Safe to call from task context only (uses kmm_malloc).
 *   If `tid` is already registered the existing entry is returned.
 *
 * Returned Value:
 *   Pointer to the entry, or NULL on allocation failure.
 *
 ****************************************************************************/

static FAR struct task_notify_entry_s *
task_notify_register(pid_t tid)
{
  FAR struct task_notify_entry_s *entry;
  FAR struct task_notify_entry_s *existing;
  irqstate_t flags;

  /* Allocate and initialise outside the critical section so that kmm_malloc
   * and nxsem_init are not called with IRQs disabled.
   */

  entry = kmm_malloc(sizeof(*entry));
  if (entry == NULL)
    {
      return NULL;
    }

  entry->tid = tid;
  nxsem_init(&entry->sem, 0, 0);

  flags = up_irq_save();

  /* Guard against a duplicate that could arise from a lazy-register race. */

  existing = task_notify_find_locked(tid);
  if (existing != NULL)
    {
      up_irq_restore(flags);
      nxsem_destroy(&entry->sem);
      kmm_free(entry);
      return existing;
    }

  entry->next       = g_task_notify_head;
  g_task_notify_head = entry;

  up_irq_restore(flags);
  return entry;
}

/* Wrapper to adapt FreeRTOS-style void(*)(void*) to NuttX int(*)(int, char**) */

struct task_wrapper_args_s
{
  void (*func)(void *);
  void *arg;
};

static int task_wrapper_entry(int argc, FAR char *argv[])
{
  FAR struct task_wrapper_args_s *wrapper_args;
  uintptr_t ptr;

  /* argv[0] is the task name; argv[1] is the wrapper_args pointer as string */

  if (argc < 2 || argv[1] == NULL)
    {
      return -EINVAL;
    }

  ptr = (uintptr_t)strtoul(argv[1], NULL, 0);
  wrapper_args = (FAR struct task_wrapper_args_s *)ptr;

  wrapper_args->func(wrapper_args->arg);
  kmm_free(wrapper_args);
  return 0;
}

int esp_os_create_task_pinned_to_core(esp_os_task_function_t task_func,
                                      const char *name,
                                      uint32_t stack_size,
                                      void *arg,
                                      int priority,
                                      esp_os_task_handle_t *task_handle,
                                      int core_id)
{
  FAR struct tcb_s *tcb;
  FAR struct task_wrapper_args_s *wrapper_args;
  FAR char *argv[2];
  char ptr_buf[32];
  int ret;

  wrapper_args = kmm_zalloc(sizeof(struct task_wrapper_args_s));
  if (wrapper_args == NULL)
    {
      return -1;
    }

  wrapper_args->func = (void (*)(void *))task_func;
  wrapper_args->arg  = arg;

  snprintf(ptr_buf, sizeof(ptr_buf), "%p", (FAR void *)wrapper_args);
  argv[0] = ptr_buf;
  argv[1] = NULL;

  tcb = kmm_zalloc(sizeof(struct tcb_s));
  if (tcb == NULL)
    {
      kmm_free(wrapper_args);
      return -1;
    }

  tcb->flags = TCB_FLAG_TTYPE_KERNEL | TCB_FLAG_FREE_TCB;

  ret = nxtask_init(tcb, name, priority,
                    NULL, stack_size,
                    task_wrapper_entry, argv, NULL, NULL);
  if (ret < 0)
    {
      kmm_free(wrapper_args);
      kmm_free(tcb);
      return -1;
    }

#ifdef CONFIG_SMP
  if (core_id >= 0)
    {
      cpu_set_t cpuset;
      CPU_ZERO(&cpuset);
      CPU_SET(core_id, &cpuset);
      tcb->affinity = cpuset;
    }
#endif

  if (task_notify_register(tcb->pid) == NULL)
    {
      _warn("esp_os_create_task: out of memory for notify entry '%s'\n",
            name);
    }

  if (task_handle != NULL)
    {
      *task_handle = (esp_os_task_handle_t)tcb->pid;
    }

  nxtask_activate(tcb);
  return 0;
}

void esp_os_task_delete(esp_os_task_handle_t handle)
{
  FAR struct task_notify_entry_s *entry;
  FAR struct task_notify_entry_s *prev;
  irqstate_t flags;

  if (handle > 0)
    {
      /* Unlink the entry under IRQ lock so that the slot is immediately
       * unreachable from ISR context, then destroy the semaphore and free
       * the memory outside the lock (kmm_free is not ISR-safe).
       */

      flags = up_irq_save();

      prev  = NULL;
      entry = g_task_notify_head;
      while (entry != NULL)
        {
          if (entry->tid == (pid_t)handle)
            {
              if (prev != NULL)
                {
                  prev->next = entry->next;
                }
              else
                {
                  g_task_notify_head = entry->next;
                }

              break;
            }

          prev  = entry;
          entry = entry->next;
        }

      up_irq_restore(flags);

      if (entry != NULL)
        {
          nxsem_destroy(&entry->sem);
          kmm_free(entry);
        }

      nxtask_delete((pid_t)handle);
    }
}

uint32_t esp_os_task_notify_take(bool clear_on_exit, uint32_t wait_ticks)
{
  FAR struct task_notify_entry_s *entry;
  irqstate_t flags;
  pid_t tid;
  int ret;

  tid = gettid();

  /* Look up our own entry; register lazily for tasks not created through
   * esp_os_create_task_pinned_to_core.
   */

  flags = up_irq_save();
  entry = task_notify_find_locked(tid);
  up_irq_restore(flags);

  if (entry == NULL)
    {
      entry = task_notify_register(tid);
    }

  if (entry == NULL)
    {
      _err("esp_os_task_notify_take: out of memory – cannot wait\n");
      return 0;
    }

  /* Wait for a notification. */

  if (wait_ticks == OS_PORT_MAX_DELAY)
    {
      ret = nxsem_wait(&entry->sem);
    }
  else
    {
      struct timespec abstime;
      struct timespec ts;

      clock_gettime(CLOCK_REALTIME, &abstime);
      clock_ticks2time(&ts, wait_ticks);
      clock_timespec_add(&abstime, &ts, &abstime);

      ret = nxsem_timedwait(&entry->sem, &abstime);
    }

  /* Binary-semaphore semantics: drain any extra counts accumulated while
   * this task was executing its callback.
   */

  if (ret == OK && clear_on_exit)
    {
      while (nxsem_trywait(&entry->sem) == OK)
        {
          /* drain */
        }
    }

  return (ret == OK) ? 1 : 0;
}

int esp_os_task_notify_give_from_isr(esp_os_task_handle_t task,
                                      FAR int *higher_priority_woken)
{
  FAR struct task_notify_entry_s *entry;
  irqstate_t flags;
  int ret = -ENOENT;

  flags = up_irq_save();
  entry = task_notify_find_locked((pid_t)task);
  up_irq_restore(flags);

  if (entry != NULL)
    {
      ret = nxsem_post(&entry->sem);
    }
  else
    {
      _warn("notify_give_from_isr: no entry for task %d\n", (int)task);
    }

  if (higher_priority_woken != NULL)
    {
      *higher_priority_woken = 0;
    }

  return ret;
}

esp_os_task_handle_t esp_os_task_get_current_handle(void)
{
  return gettid();
}

void esp_os_task_delay_ms(uint32_t ms)
{
  usleep(ms * 1000);
}

uint32_t esp_os_task_get_tick_count(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (uint32_t)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
}
