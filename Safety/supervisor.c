#include "app.h"

#include "iwdg.h"

extern IWDG_HandleTypeDef hiwdg;

/**
 * @brief Supervisor task main function
 * 
 * @param pvParameters 
 */
void supervisor_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime;

    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount();
        // HAL_IWDG_Refresh(&hiwdg);
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_SUPERVISOR_TASK_PERIOD_MS));

    }
}