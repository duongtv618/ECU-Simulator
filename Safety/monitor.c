#include "app.h"

/**
 * @brief Monitor task main function
 * 
 * @param pvParameters Unuse
 */
void monitor_task(void * pvParameters)
{
    (void) pvParameters;
    uint32_t lastTick = xTaskGetTickCount();
    while (1)
    {
        vTaskDelayUntil(&lastTick, pdMS_TO_TICKS(APP_MONITOR_TASK_PERIOD_MS));
    }
}