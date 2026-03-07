#include "app.h"

#include "stdio.h"

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
    TickType_t xLastWakeTime;//Wake time
    char msg[20];//Buffer for sprintf, i should create a itoa function

    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount();
        led_toggle();
        os_debug_put_string(mess);
        os_debug_put_string("Control task jitter:");
        os_debug_put_string(os_itoa(os_getMaxJitter(control_task_handle),msg, 10));
        os_debug_put_ln();
        os_debug_put_string("Supervisor task jitter:");
        os_debug_put_string(os_itoa(os_getMaxJitter(supervisor_task_handle),msg, 10));
        os_debug_put_ln();
        os_debug_put_string("Sensor task jitter:");
        os_debug_put_string(os_itoa(os_getMaxJitter(sensor_task_handle),msg, 10));
        os_debug_put_ln();
        os_debug_put_string("Comm task jitter:");
        os_debug_put_string(os_itoa(os_getMaxJitter(comm_task_handle),msg, 10));
        os_debug_put_ln();
        
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_COMM_TASK_PERIOD_MS));

    }
}