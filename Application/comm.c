#include "app.h"

#include "stdio.h"

/**
 * @brief Extern task handle from app.h
 * 
 */
extern TaskHandle_t supervisor_task_handle;
extern TaskHandle_t sensor_task_handle;
extern TaskHandle_t comm_task_handle;

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
    char msg[50];//Buffer for sprintf, i should create a itoa function

    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount();

        os_debug_put_string(mess);
        sprintf(msg, "Max jitter supervisor: %lu\r\n", os_getMaxJitter(comm_task_handle));
        os_debug_put_string(msg);
        sprintf(msg, "Max jitter comm: %lu\r\n", os_getMaxJitter(comm_task_handle));
        os_debug_put_string(msg);
        sprintf(msg, "Max jitter sensor: %lu\r\n", os_getMaxJitter(comm_task_handle));
        os_debug_put_string(msg);
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_COMM_TASK_PERIOD_MS));

    }
}