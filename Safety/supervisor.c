#include "app.h"

#include "iwdg.h"

#define BITS_TO_WAIT (APP_SENSOR_TASK_HB_BIT | APP_CONTROL_TASK_HB_BIT)
#define RAM_PATTEN_SIZE 8U
#define PATTERN 0xA5A5A5A5

extern uint32_t g_alive;

static uint32_t ram_pattern[RAM_PATTEN_SIZE];

static void ram_pattern_write(void);
static int ram_pattern_check(void);

/**
 * @brief Supervisor task main function
 *
 * @param pvParameters
 */
void supervisor_task(void *pvParameters)
{
    (void)pvParameters;
    ram_pattern_write();
    TickType_t lastTick = xTaskGetTickCount();
    for (;;)
    {
        if (ram_pattern_check())
        {
            //RAM corrupt, log fault then reboot to fail state
            //LOG_FAULT(RAM_PATTERN_NOT_PASS);
            // NVIC_SystemReset();
        }
        if ((g_alive & BITS_TO_WAIT) == BITS_TO_WAIT){
            //System healthy
            g_alive = 0;
            iwdg_refresh();
        } else {
            os_log(OS_DEBUG_CONSOLE,(uint8_t*) "Task not alive\r\n", 0);
            /** Can supend task here, if still panic do a hard reset
             * after hard reset if the problem is still there, the
             * machine have fatal broken, it is hardware problem now
             */
            while (1){
                iwdg_refresh();
                for (size_t i = 0; i < 10000; i++);
                led_toggle();
            }
            /** Can add a counter here if supend broken task can not solve the
             * problem. When couner come, just log to pernament memmory then the next 
             * bootup will jump to error state
             */
        }

        vTaskDelayUntil(&lastTick, pdMS_TO_TICKS(APP_SUPERVISOR_TASK_PERIOD_MS));
    }
}


static int ram_pattern_check(void)
{
    for (size_t i = 0; i < RAM_PATTEN_SIZE; i++)
    {
        if (ram_pattern[i] != PATTERN)
        {
            return 1;
        }       
    }
    return 0;
}
static void ram_pattern_write(void)
{
    for (size_t i = 0; i < RAM_PATTEN_SIZE; i++)
    {
        ram_pattern[i] = 0xA5A5A5A5;
    }
}