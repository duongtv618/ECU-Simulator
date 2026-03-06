#ifndef APP_H
#define APP_H


#include "app_cfg.h"
#include "os.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"



/**
 * Global variables and definitions for the application
 */
extern struct sensor_data_s
{
    SemaphoreHandle_t data_mutex;
    uint16_t adc_value;
} g_sensor_data;

extern TaskHandle_t sensor_task_handle;
extern TaskHandle_t comm_task_handle;
extern TaskHandle_t supervisor_task_handle;
/**
 * Task function prototypes
 */
void sensor_task(void *pvParameters);
void comm_task(void *pvParameters);
void control_task(void *pvParameters);
void supervisor_task(void *pvParameters);




void app_init(void);
void app_start(void);

#endif /* APP_H */