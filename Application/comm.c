#include "app.h"

#include <string.h>

/**
 * @brief Extern task handle from app.h
 * 
 */
extern TaskHandle_t supervisor_task_handle;
extern TaskHandle_t sensor_task_handle;
extern TaskHandle_t comm_task_handle;
extern TaskHandle_t control_task_handle;

/** Notice string */
const char mess[] = "ECU Simulator is running...\r\n";

/**
 * @brief Communication task main function
 * 
 * @param pvParameters Unuse
 */
void comm_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();//Wake time
    char msg[50];

    for (;;)
    {
        led_toggle();
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)mess, 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"Control task jitter: ", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(os_getMaxJitter(control_task_handle),msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)", min jitter: ", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(jitter_get_min_jitter(control_task_handle), msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)", max executed time: ", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(jitter_get_max_exec_time(control_task_handle), msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"\r\n", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"Supervisor task jitter:", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(os_getMaxJitter(supervisor_task_handle),msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)", min jitter: ", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(jitter_get_min_jitter(supervisor_task_handle), msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)", max executed time: ", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(jitter_get_max_exec_time(supervisor_task_handle), msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"\r\n", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"Sensor task jitter:", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(os_getMaxJitter(sensor_task_handle),msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)", min jitter: ", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(jitter_get_min_jitter(sensor_task_handle), msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)", max executed time: ", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(jitter_get_max_exec_time(sensor_task_handle), msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"\r\n", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"Comm task jitter:", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(os_getMaxJitter(comm_task_handle),msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)", min jitter: ", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(jitter_get_min_jitter(comm_task_handle), msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)", max executed time: ", 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)os_itoa(jitter_get_max_exec_time(comm_task_handle), msg, 10), 0);
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"\r\n", 0);
        
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_COMM_TASK_PERIOD_MS));

    }
}