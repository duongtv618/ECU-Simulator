#ifndef OS_H
#define OS_H

#include "os_cfg.h"

#include "adc.h"
#include "iwdg.h"
#include "usart.h"
#include "gpio.h"
#include "jitter.h"
#include "timer_pwm.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

typedef enum {
    OS_DEBUG_CONSOLE,
    OS_FLASH,
    OS_RTC_REGISTER
} os_log_type_t;

extern QueueHandle_t g_adc_queue;


void os_init(void);
void os_log(os_log_type_t type, uint8_t * data, uint16_t len);
void os_registerJitterTask(TaskHandle_t tskHandler, uint16_t periodMS);
uint32_t os_getMaxJitter(TaskHandle_t task);
uint32_t os_getAccumExeTime(TaskHandle_t task);
char *os_itoa(int32_t num, char* str, int32_t base);

void adc_start_sampling(void);
void adc_stop_sampling(void);


#endif /* OS_H */