#ifndef OS_H
#define OS_H


#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "adc.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "FreeRTOS.h"
#include "queue.h"

#define IPC_REGION __attribute__((section(".ipc_region")))

extern QueueHandle_t g_adc_queue;


void adc_start_sampling(void);
void adc_stop_sampling(void);
void SystemClock_Config(void);


#endif /* OS_H */