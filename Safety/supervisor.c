#include "app.h"

#include "iwdg.h"

#define BITS_TO_WAIT (APP_SENSOR_TASK_HB_BIT | APP_CONTROL_TASK_HB_BIT)

extern EventGroupHandle_t g_hb_evengroup;
/**
 * @brief Supervisor task main function
 * 
 * @param pvParameters 
 */
void supervisor_task(void *pvParameters)
{
    (void)pvParameters;
    // TickType_t xLastWakeTime;

    for (;;)
    {
        // xLastWakeTime = xTaskGetTickCount();
        // iwdg_refresh();
        /* Wait for the next cycle */
        EventBits_t retBit = xEventGroupWaitBits(g_hb_evengroup, BITS_TO_WAIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(APP_SUPERVISOR_TASK_PERIOD_MS));

        if ((retBit & BITS_TO_WAIT) == BITS_TO_WAIT) {
            //System ok
        } else if ((retBit & APP_SENSOR_TASK_HB_BIT) == 0) {
            //Sensor task fail
            while(1);
        } else if ((retBit & APP_CONTROL_TASK_HB_BIT) == 0) {
            //Control task fail
            while(1);
        } else {
            //Both are not being set
            while(1);
        }
    }
}