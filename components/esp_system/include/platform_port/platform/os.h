#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define OS_PORT_MAX_DELAY portMAX_DELAY
#define OS_PORT_TICKS_TO_MS pdMS_TO_TICKS
#define OS_BASE_TYPE BaseType_t
#define OS_FALSE pdFALSE
#define OS_TRUE pdTRUE
#define OS_PORT_NUM_PROCESSORS portNUM_PROCESSORS
#define OS_TASK_DELAY(ticks) vTaskDelay(ticks)
#define OS_SCHEDULER_RUNNING() xTaskGetSchedulerState() == taskSCHEDULER_RUNNING

#if ( ( CONFIG_FREERTOS_SMP ) && ( !CONFIG_FREERTOS_UNICORE ) )
  //Note: Scheduler suspension behavior changed in FreeRFTOS SMP
#define  OS_PORT_SUSPEND_SCHEDULER() vTaskPreemptionDisable(NULL);
#else
  // Disable scheduler on this core.
#define  OS_PORT_SUSPEND_SCHEDULER() vTaskSuspendAll();
#endif // #if ( ( CONFIG_FREERTOS_SMP ) && ( !CONFIG_FREERTOS_UNICORE ) )

#define esp_os_spinlock_initialize(lock) spinlock_initialize(lock)

typedef QueueHandle_t esp_os_queue_handle_t;
typedef TickType_t esp_os_tick_type_t;
typedef intr_handle_t esp_os_intr_handle_t;
typedef intr_handler_t esp_os_intr_handler_t;
