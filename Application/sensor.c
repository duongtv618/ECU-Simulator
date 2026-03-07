#include "app.h"

extern QueueHandle_t g_adc_queue;
extern struct sensor_data_s g_sensor_data;
extern EventGroupHandle_t g_hb_evengroup;

void sensor_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime;
    uint16_t adc_value;

    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount();
        xEventGroupSetBits(g_hb_evengroup, APP_SENSOR_TASK_HB_BIT);
        /* Update shared sensor data */
        xQueueReceive(g_adc_queue, &adc_value, portMAX_DELAY);
        /** Update sensor data, don't update if mutex is being locked */
        if (xSemaphoreTake(g_sensor_data.data_mutex, 0) == pdPASS)
        {
            g_sensor_data.adc_value = adc_value;
            xSemaphoreGive(g_sensor_data.data_mutex);
        }
        /** Start adc conversion */
        adc_start_sampling();
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_SENSOR_TASK_PERIOD_MS));
    }
}