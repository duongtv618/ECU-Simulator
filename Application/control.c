#include "app.h"


#define LOOKUP(x) (x)

extern EventGroupHandle_t g_hb_evengroup;

void control_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime;
    uint32_t adcValue;
    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount();
        xEventGroupSetBits(g_hb_evengroup, APP_CONTROL_TASK_HB_BIT);
        /** Take mutex, cause control task have higher priority, no need to check */
        xSemaphoreTake(g_sensor_data.data_mutex, portMAX_DELAY);
        adcValue = g_sensor_data.adc_value;
        xSemaphoreGive(g_sensor_data.data_mutex);

        /** Calculation here */
        uint32_t pwmOut = LOOKUP(adcValue);//With 512Byte ROM at lease 256KB can be used as lookup table, amazing

        /** Write to PWM timer */
        // os_write_pwmTimer(pwmOut);

        (void) pwmOut;

#ifdef INJECT_TASK_OVER_PERIOD
        for (size_t i = 0; i < 100000; i++);
#endif
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_CONTROL_TASK_PERIOD_MS));
    }
}