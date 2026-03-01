#include "app.h"


void comm_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime;

    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount();
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_COMM_TASK_PERIOD_MS));

    }
}