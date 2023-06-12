/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "sdkconfig.h"

#include <nuttx/config.h>
#include <nuttx/arch.h>
#include <nuttx/init.h>
#include <nuttx/mutex.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <debug.h>

#include <sched/sched.h>

#include "esp_err.h"
#include "esp_ipc.h"
#include "esp_cpu.h"

#include "rom/ets_sys.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#if CONFIG_ESP_IPC_ENABLE

#ifndef CONFIG_ESP_IPC_TASK_STACK_SIZE
#define CONFIG_ESP_IPC_TASK_STACK_SIZE 1280
#endif

#if CONFIG_COMPILER_OPTIMIZATION_NONE
#define IPC_STACK_SIZE (CONFIG_ESP_IPC_TASK_STACK_SIZE + 0x100)
#else
#define IPC_STACK_SIZE (CONFIG_ESP_IPC_TASK_STACK_SIZE)
#endif /* CONFIG_COMPILER_OPTIMIZATION_NONE */

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef enum
{
  IPC_WAIT_NO = 0,
  IPC_WAIT_FOR_START,
  IPC_WAIT_FOR_END,
} esp_ipc_wait_t;

/****************************************************************************
 * Private Data
 ****************************************************************************/

static FAR struct tcb_s *s_ipc_task_handle[CONFIG_SMP_NCPUS];
static mutex_t s_ipc_mutex[CONFIG_SMP_NCPUS];
static sem_t s_ipc_ack[CONFIG_SMP_NCPUS];
static sem_t s_ipc_notify[CONFIG_SMP_NCPUS];
static volatile esp_ipc_func_t s_func[CONFIG_SMP_NCPUS];
static void * volatile s_func_arg[CONFIG_SMP_NCPUS];
static esp_ipc_wait_t volatile s_wait_for[CONFIG_SMP_NCPUS];

static volatile esp_ipc_func_t s_no_block_func[CONFIG_SMP_NCPUS];
static volatile bool s_no_block_func_and_arg_are_ready[CONFIG_SMP_NCPUS];
static void * volatile s_no_block_func_arg[CONFIG_SMP_NCPUS];

static volatile bool s_ipc_initialized = false;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: ipc_task
 *
 * Description:
 *   The IPC task that runs on each CPU and waits for IPC requests.
 *
 ****************************************************************************/

static int ipc_task(int argc, char *argv[])
{
  int cpuid = this_cpu();
  FAR struct tcb_s *tcb = this_task();

  DEBUGASSERT((1 << cpuid) & tcb->affinity);

  while (true)
    {
      /* Wait for notification to process IPC call */

      nxsem_wait(&s_ipc_notify[cpuid]);

      if (s_no_block_func_and_arg_are_ready[cpuid] && s_no_block_func[cpuid])
        {
          (*s_no_block_func[cpuid])(s_no_block_func_arg[cpuid]);
          s_no_block_func_and_arg_are_ready[cpuid] = false;
          s_no_block_func[cpuid] = NULL;
        }

#if CONFIG_SMP_NCPUS > 1
      if (s_func[cpuid])
        {
          /* We need to cache s_func, s_func_arg and ipc_ack variables
           * locally because they can be changed by a subsequent IPC call.
           */

          esp_ipc_func_t func = s_func[cpuid];
          void *func_arg = s_func_arg[cpuid];
          esp_ipc_wait_t ipc_wait = s_wait_for[cpuid];
          s_func[cpuid] = NULL;

          if (ipc_wait == IPC_WAIT_FOR_START)
            {
              nxsem_post(&s_ipc_ack[cpuid]);
              (*func)(func_arg);
            }
          else if (ipc_wait == IPC_WAIT_FOR_END)
            {
              (*func)(func_arg);
              nxsem_post(&s_ipc_ack[cpuid]);
            }
          else
            {
              PANIC();
            }
        }
#endif /* CONFIG_SMP_NCPUS > 1 */
    }

  return OK;
}

/****************************************************************************
 * Name: esp_ipc_init_task
 *
 * Description:
 *   Starts a kernel thread for IPC on a specific CPU.
 *
 ****************************************************************************/

static int esp_ipc_init_task(int cpu)
{
  FAR struct tcb_s *tcb;
  int ret;
  char *argv[2];
  char arg1[32];
  cpu_set_t cpuset;
  char task_name[] = "ipcX";

  task_name[3] = cpu + (char)'0';

  snprintf(arg1, sizeof(arg1), "%d", cpu);
  argv[0] = arg1;
  argv[1] = NULL;

  /* Allocate a TCB for the new task */

  tcb = kmm_zalloc(sizeof(struct tcb_s));
  if (!tcb)
    {
      _err("ERROR: Failed to allocate TCB for IPC task\n");
      return -ENOMEM;
    }

  /* Setup the task type */

  tcb->flags = TCB_FLAG_TTYPE_KERNEL | TCB_FLAG_FREE_TCB;

  /* Initialize the task */

  ret = nxtask_init(tcb, task_name,
                    SCHED_PRIORITY_MAX,
                    NULL, IPC_STACK_SIZE,
                    ipc_task, argv, environ, NULL);
  if (ret < OK)
    {
      kmm_free(tcb);
      _err("ERROR: nxtask_init failed for IPC task: %d\n", ret);
      return ret;
    }

  /* Set the affinity to pin to specific CPU */

  CPU_ZERO(&cpuset);
  CPU_SET(cpu, &cpuset);
  tcb->affinity = cpuset;

  /* Store the task handle */

  s_ipc_task_handle[cpu] = tcb;

  /* Activate the task */

  nxtask_activate(tcb);

  return OK;
}

/****************************************************************************
 * Name: esp_ipc_init
 *
 * Description:
 *   Initialize inter-processor call module. This function creates IPC tasks
 *   on each CPU. These tasks are started with high priority and are normally
 *   inactive, waiting until one of the esp_ipc_call_* functions is used.
 *
 ****************************************************************************/

/* Note: We don't use __attribute__((constructor)) because the NuttX scheduler
 * is not ready when constructors run. Instead, we use lazy initialization in
 * esp_ipc_ensure_init() called by all public IPC functions.
 */

static void esp_ipc_init(void)
{
  int cpu;
  int ret;

  if (s_ipc_initialized)
    {
      return;
    }

  /* Ensure the scheduler is ready */

  if (!OSINIT_OS_READY())
    {
      return;
    }

  sched_lock();

  /* Double check after acquiring lock */

  if (s_ipc_initialized)
    {
      sched_unlock();
      return;
    }

  /* Initialize synchronization primitives and create tasks for each CPU */

  for (cpu = 0; cpu < CONFIG_SMP_NCPUS; cpu++)
    {
      nxmutex_init(&s_ipc_mutex[cpu]);
      nxsem_init(&s_ipc_ack[cpu], 0, 0);
      nxsem_init(&s_ipc_notify[cpu], 0, 0);

      ret = esp_ipc_init_task(cpu);
      DEBUGASSERT(ret == OK);
      (void)ret;
    }

  s_ipc_initialized = true;

  sched_unlock();

  sinfo("IPC tasks created successfully\n");
}

/****************************************************************************
 * Name: esp_ipc_ensure_init
 *
 * Description:
 *   Ensure IPC is initialized. Called lazily if constructor didn't run
 *   when scheduler was ready.
 *
 ****************************************************************************/

static void esp_ipc_ensure_init(void)
{
  if (!s_ipc_initialized && OSINIT_OS_READY())
    {
      esp_ipc_init();
    }
}

/****************************************************************************
 * Name: esp_ipc_call_and_wait
 *
 * Description:
 *   Execute a function on another CPU and wait.
 *
 ****************************************************************************/

static esp_err_t esp_ipc_call_and_wait(uint32_t cpu_id, esp_ipc_func_t func,
                                       void *arg, esp_ipc_wait_t wait_for)
{
  FAR struct tcb_s *current_task;

  esp_ipc_ensure_init();

  if (cpu_id >= CONFIG_SMP_NCPUS)
    {
      return ESP_ERR_INVALID_ARG;
    }

  if (s_ipc_task_handle[cpu_id] == NULL)
    {
      return ESP_ERR_INVALID_STATE;
    }

  if (!OSINIT_OS_READY())
    {
      return ESP_ERR_INVALID_STATE;
    }

  current_task = this_task();

  /* Check for recursion: if we're already the IPC task, run directly */

  if (current_task == s_ipc_task_handle[cpu_id])
    {
      func(arg);
      return ESP_OK;
    }

  /* Take mutex to serialize IPC calls */

  nxmutex_lock(&s_ipc_mutex[cpu_id]);

  s_func_arg[cpu_id] = arg;
  s_wait_for[cpu_id] = wait_for;

  /* s_func must be set after all other parameters.
   * The ipc_task uses this as indicator of the IPC being prepared.
   */

  s_func[cpu_id] = func;

  /* Notify the IPC task */

  nxsem_post(&s_ipc_notify[cpu_id]);

  /* Wait for acknowledgement */

  nxsem_wait(&s_ipc_ack[cpu_id]);

  nxmutex_unlock(&s_ipc_mutex[cpu_id]);

  return ESP_OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: esp_ipc_call
 *
 * Description:
 *   Execute a callback on a given CPU.
 *   This function will block until the target CPU's IPC task has begun
 *   execution of the callback.
 *
 ****************************************************************************/

esp_err_t esp_ipc_call(uint32_t cpu_id, esp_ipc_func_t func, void *arg)
{
  return esp_ipc_call_and_wait(cpu_id, func, arg, IPC_WAIT_FOR_START);
}

/****************************************************************************
 * Name: esp_ipc_call_blocking
 *
 * Description:
 *   Execute a callback on a given CPU until and block until it completes.
 *
 ****************************************************************************/

esp_err_t esp_ipc_call_blocking(uint32_t cpu_id, esp_ipc_func_t func,
                                void *arg)
{
  return esp_ipc_call_and_wait(cpu_id, func, arg, IPC_WAIT_FOR_END);
}

/****************************************************************************
 * Name: esp_ipc_call_nonblocking
 *
 * Description:
 *   Execute a callback on a given CPU without blocking.
 *   Can be called from ISR context.
 *
 ****************************************************************************/

esp_err_t esp_ipc_call_nonblocking(uint32_t cpu_id, esp_ipc_func_t func,
                                   void *arg)
{
  esp_ipc_ensure_init();

  if (cpu_id >= CONFIG_SMP_NCPUS || s_ipc_task_handle[cpu_id] == NULL)
    {
      return ESP_ERR_INVALID_ARG;
    }

  if (cpu_id == (uint32_t)this_cpu() && !OSINIT_OS_READY())
    {
      return ESP_ERR_INVALID_STATE;
    }

  /* Since it can be called from an interrupt or when scheduler is suspended,
   * it cannot wait for a mutex to be released.
   */

  if (esp_cpu_compare_and_set((volatile uint32_t *)&s_no_block_func[cpu_id],
                              0, (uint32_t)func))
    {
      s_no_block_func_arg[cpu_id] = arg;
      s_no_block_func_and_arg_are_ready[cpu_id] = true;

      /* Post to the notify semaphore to wake the IPC task */

      nxsem_post(&s_ipc_notify[cpu_id]);

      return ESP_OK;
    }

  /* The previous call was not completed */

  return ESP_FAIL;
}

#endif /* CONFIG_ESP_IPC_ENABLE */
