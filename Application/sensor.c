#include "app.h"

extern QueueHandle_t g_adc_queue;

void sensor_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime;
    uint16_t adc_value;

    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount();
        /* Update shared sensor data */
        xQueueReceive(g_adc_queue, &adc_value, portMAX_DELAY);
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_SENSOR_TASK_PERIOD_MS));
    }
}