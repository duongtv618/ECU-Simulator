#ifndef OS_H
#define OS_H

#include "os_cfg.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "adc.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "jitter.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

extern QueueHandle_t g_adc_queue;


void os_init(void);
void os_debug_put_string(const char *str) SYSTEMCALL;
void os_debug_put_ln(void);
void os_registerJitterTask(TaskHandle_t tskHandler, uint16_t periodMS);
uint32_t os_getMaxJitter(TaskHandle_t task);
char *os_itoa(int32_t num, char* str, int32_t base);

void adc_start_sampling(void);
void adc_stop_sampling(void);


#endif /* OS_H */