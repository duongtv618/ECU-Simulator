#include "app.h"

void control_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime;

    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount();
        /* Control logic can be implemented here, using g_sensor_data */
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_CONTROL_TASK_PERIOD_MS));
    }
}