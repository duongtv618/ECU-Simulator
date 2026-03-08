#include "app.h"

extern QueueHandle_t g_adc_queue;
extern struct sensor_data_s g_sensor_data;
extern uint32_t g_alive;

void sensor_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint16_t adc_value;

    for (;;)
    {
        /** Start adc conversion */
        adc_start_sampling();
        if (adc_poll_for_eoc(10000)){
            //handle error here
            while (1);
        }
        adc_value = adc_get_value();
        /* Update shared sensor data */
        // xQueueReceive(g_adc_queue, &adc_value, portMAX_DELAY);
        /** Update sensor data, don't update if mutex is being locked */
        if (xSemaphoreTake(g_sensor_data.data_mutex, 0) == pdPASS){
            g_sensor_data.adc_value = adc_value;
            xSemaphoreGive(g_sensor_data.data_mutex);
        }
        /** Set alive bit */
        // xEventGroupSetBits(g_hb_evengroup, APP_SENSOR_TASK_HB_BIT);
        g_alive |= APP_SENSOR_TASK_HB_BIT;
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_SENSOR_TASK_PERIOD_MS));
    }
}