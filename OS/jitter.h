#ifndef JITTER_H
#define JITTER_H

#include "FreeRTOS.h"
#include "task.h"


void jitter_init(void);
extern void trace_task_switch_in(void);
void jitter_add_task(TaskHandle_t tsk, uint32_t period);
uint32_t jitter_get_max_jitter(TaskHandle_t task);

#endif