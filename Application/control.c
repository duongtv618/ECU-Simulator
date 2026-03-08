#include "app.h"


#define LOOKUP(x) (x)
#define RANGE(x, x1, x2, y1, y2) ((x)*(y2-y1)/(x2-x1))

extern uint32_t g_alive;

void control_task(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();;
    uint32_t adcValue;

    timer_pwm_start();
    for (;;)
    {
        /** Take mutex, cause control task have higher priority, no need to check */
        xSemaphoreTake(g_sensor_data.data_mutex, portMAX_DELAY);
        adcValue = g_sensor_data.adc_value;
        xSemaphoreGive(g_sensor_data.data_mutex);

        /** Calculation here */
        uint32_t pwmOut = LOOKUP(adcValue);//With 512Byte ROM at least 256KB can be used as lookup table, amazing

        /** Write to PWM timer */
        timer_pwm_write(RANGE(pwmOut, 0, 0x0FFF, 0, 99));
        g_alive |= APP_CONTROL_TASK_HB_BIT;
        /* Wait for the next cycle */
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_CONTROL_TASK_PERIOD_MS));
    }
}