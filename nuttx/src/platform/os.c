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
#include <clock/clock.h>

#include <nuttx/mutex.h>
#include <nuttx/spinlock.h>
#include <nuttx/irq.h>
#include <nuttx/queue.h>
#include <nuttx/mqueue.h>
#include <nuttx/kmalloc.h>

#include "sdkconfig.h"

#include "esp_irq.h"

#include "esp_private/critical_section.h"

#include "platform/os.h"

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
