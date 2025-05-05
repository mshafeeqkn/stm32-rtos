#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION            1
#define configCPU_CLOCK_HZ              (72000000UL)
#define configTICK_RATE_HZ              (1000)
#define configMAX_PRIORITIES            4
#define configMINIMAL_STACK_SIZE        (128)
#define configTOTAL_HEAP_SIZE           (10240)
#define configUSE_16_BIT_TICKS          0
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0

#define configMAX_TASK_NAME_LEN         16
#define configUSE_MALLOC_FAILED_HOOK    0
#define configCHECK_FOR_STACK_OVERFLOW  0
#define configQUEUE_REGISTRY_SIZE       0

#include <stdint.h>
#define configASSERT(x) if((x)==0){taskDISABLE_INTERRUPTS(); for(;;);}

#endif

