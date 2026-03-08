#include "app.h"


#define LOOKUP(x) (x)

extern uint32_t g_alive;

void control_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();;
    uint32_t adcValue;
    for (;;)
    {
        /** Take mutex, cause control task have higher priority, no need to check */
        xSemaphoreTake(g_sensor_data.data_mutex, portMAX_DELAY);
        adcValue = g_sensor_data.adc_value;
        xSemaphoreGive(g_sensor_data.data_mutex);

        /** Calculation here */
        uint32_t pwmOut = LOOKUP(adcValue);//With 512Byte ROM at least 256KB can be used as lookup table, amazing

        /** Write to PWM timer */
        // os_write_pwmTimer(pwmOut);

        (void) pwmOut;

#ifdef INJECT_TASK_OVER_PERIOD
        for (size_t i = 0; i < 100000; i++);
#endif
        g_alive |= APP_CONTROL_TASK_HB_BIT;
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_CONTROL_TASK_PERIOD_MS));
    }
}