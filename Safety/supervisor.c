#include "app.h"

#include "iwdg.h"

#define BITS_TO_WAIT (APP_SENSOR_TASK_HB_BIT | APP_CONTROL_TASK_HB_BIT)

extern uint32_t g_alive;
/**
 * @brief Supervisor task main function
 *
 * @param pvParameters
 */
void supervisor_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t lastTick = xTaskGetTickCount();
    for (;;)
    {
        if ((g_alive & BITS_TO_WAIT) == BITS_TO_WAIT){
            //System healthy
            g_alive = 0;
        } else {
            while (1);
        }

        vTaskDelayUntil(&lastTick, pdMS_TO_TICKS(APP_SUPERVISOR_TASK_PERIOD_MS));
    }
}