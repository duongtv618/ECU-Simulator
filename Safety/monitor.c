#include "app.h"
#include "mode_man.h"

#define WARNING_THRESH_HOLD(x) (x / 5)
#define FAIL_THRESH_HOLD(x) (x / 10)
#define CPU_LOAD_WARNING_THR 70U
#define CPU_LOAD_FAIL_THR 85U

extern TaskHandle_t control_task_handle;
extern TaskHandle_t supervisor_task_handle;
extern TaskHandle_t comm_task_handle;
extern TaskHandle_t sensor_task_handle;

static struct monitor_stack_s
{
    UBaseType_t controlTaskMaxWaterMark;
    UBaseType_t supervisorTaskMaxWaterMark;
    UBaseType_t commTaskMaxStackWaterMark;
    UBaseType_t sensorTaskMaxStackWaterMark;
} s_monitor = {
    APP_CONTROL_TASK_STACK_SIZE,
    APP_SUPERVISOR_TASK_STACK_SIZE,
    APP_COMM_TASK_STACK_SIZE,
    APP_SENSOR_TASK_STACK_SIZE};

static BaseType_t idleHookCounter = 0;

void check_peak_stack_useage(void);
void vApplicationStackOverflowHook(void);
void vApplicationIdleHook(void);
void monitor_checkCPULoad(uint32_t cpuLoad);

/**
 * @brief Monitor task main function
 * Monitor CPU useage and stack water mark monitor
 *
 * @param pvParameters Unuse
 */
void monitor_task(void *pvParameters)
{
    (void)pvParameters;
    uint32_t lastTick = xTaskGetTickCount();
    UBaseType_t waterMark;
    uint16_t counter = 0;
    while (1)
    {
        /** Self check */
        if (uxTaskGetStackHighWaterMark2(NULL) < FAIL_THRESH_HOLD(APP_MONITOR_TASK_STACK_SIZE))
        {
            /** Monitor stack too low, system corrupted */
            mode_man_enter_mode(MODE_MAN_BROKEN);
        }

        waterMark = uxTaskGetStackHighWaterMark2(comm_task_handle);
        s_monitor.commTaskMaxStackWaterMark = (s_monitor.commTaskMaxStackWaterMark > waterMark) ? waterMark : s_monitor.commTaskMaxStackWaterMark;
        waterMark = uxTaskGetStackHighWaterMark2(control_task_handle);
        s_monitor.controlTaskMaxWaterMark = (s_monitor.controlTaskMaxWaterMark > waterMark) ? waterMark : s_monitor.controlTaskMaxWaterMark;
        waterMark = uxTaskGetStackHighWaterMark2(sensor_task_handle);
        s_monitor.sensorTaskMaxStackWaterMark = (s_monitor.sensorTaskMaxStackWaterMark > waterMark) ? waterMark : s_monitor.sensorTaskMaxStackWaterMark;
        waterMark = uxTaskGetStackHighWaterMark2(supervisor_task_handle);
        s_monitor.supervisorTaskMaxWaterMark = (s_monitor.supervisorTaskMaxWaterMark > waterMark) ? waterMark : s_monitor.supervisorTaskMaxWaterMark;

        if (++counter > 10) // 1s period
        {
            counter = 0;

            /** Check for stack useage */
            check_peak_stack_useage();

            /** Then clear */
            s_monitor.commTaskMaxStackWaterMark = APP_COMM_TASK_STACK_SIZE;
            s_monitor.controlTaskMaxWaterMark = APP_CONTROL_TASK_STACK_SIZE;
            s_monitor.sensorTaskMaxStackWaterMark = APP_SENSOR_TASK_STACK_SIZE;
            s_monitor.supervisorTaskMaxWaterMark = APP_SUPERVISOR_TASK_STACK_SIZE;
        }

        /** CPU load */
        uint32_t sensorExeTime = os_getAccumExeTime(sensor_task_handle);
        uint32_t controlExeTime = os_getAccumExeTime(control_task_handle);
        uint32_t superExeTime = os_getAccumExeTime(supervisor_task_handle);
        uint32_t commExeTime = os_getAccumExeTime(comm_task_handle);

        uint32_t cpuLoad = (sensorExeTime + controlExeTime + superExeTime + commExeTime) / (SystemCoreClock / 100U);

        monitor_checkCPULoad(cpuLoad);

        vTaskDelayUntil(&lastTick, pdMS_TO_TICKS(APP_MONITOR_TASK_PERIOD_MS));
    }
}

/** Check CPU load */
void monitor_checkCPULoad(uint32_t cpuLoad)
{
    if (cpuLoad >= CPU_LOAD_FAIL_THR)
    {
        mode_man_enter_mode(MODE_MAN_DEGRADE);
    }
    else if (cpuLoad >= CPU_LOAD_WARNING_THR)
    {
        mode_man_enter_mode(MODE_MAN_FAIL_SAFE);
    }
    else
    {
        /** CPU Load ok */
    }
}

/** FreeRTOS idle hook */
void vApplicationIdleHook(void){
    idleHookCounter++;
}

/**
 * @brief FreeRTOS application stack overflow hook
 * 
 */
void vApplicationStackOverflowHook(void)
{
    /** Have stack over flow error which is detected by FreeRTOS scheduler
     * Cause MPU is on, this hook is not neccesary, so let it be
     */

}

void check_peak_stack_useage(void)
{
    if (s_monitor.commTaskMaxStackWaterMark <= WARNING_THRESH_HOLD(APP_COMM_TASK_STACK_SIZE))
    {
        /** Comm task warning stack useage */
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"Comm task stack high useage\r\n", 0);
    }
    else if (s_monitor.commTaskMaxStackWaterMark <= FAIL_THRESH_HOLD(APP_COMM_TASK_STACK_SIZE))
    {
        /** Fail -> enter failt safe through mode manager */
        mode_man_enter_mode(MODE_MAN_FAIL_SAFE);
    }
    else
    {
        /** Stack ok */
    }

    if (s_monitor.controlTaskMaxWaterMark <= WARNING_THRESH_HOLD(APP_CONTROL_TASK_STACK_SIZE))
    {
        /** Control task warning stack useage */
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"Control task stack high useage\r\n", 0);
    }
    else if (s_monitor.controlTaskMaxWaterMark <= FAIL_THRESH_HOLD(APP_CONTROL_TASK_STACK_SIZE))
    {
        /** Fail -> enter failt safe through mode manager */
        mode_man_enter_mode(MODE_MAN_FAIL_SAFE);
    }
    else
    {
        /** Stack ok */
    }
    if (s_monitor.sensorTaskMaxStackWaterMark <= WARNING_THRESH_HOLD(APP_SENSOR_TASK_STACK_SIZE))
    {
        /** Sensor task warning stack useage */
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"Sensor task stack high useage\r\n", 0);
    }
    else if (s_monitor.sensorTaskMaxStackWaterMark <= FAIL_THRESH_HOLD(APP_SENSOR_TASK_STACK_SIZE))
    {
        /** Fail -> enter failt safe through mode manager */
        mode_man_enter_mode(MODE_MAN_FAIL_SAFE);
    }
    else
    {
        /** Stack ok */
    }

    if (s_monitor.supervisorTaskMaxWaterMark <= WARNING_THRESH_HOLD(APP_SUPERVISOR_TASK_STACK_SIZE))
    {
        /** Supervisor task warning stack useage */
        os_log(OS_DEBUG_CONSOLE, (uint8_t *)"Supervisor task stack high useage\r\n", 0);
    }
    else if (s_monitor.commTaskMaxStackWaterMark <= FAIL_THRESH_HOLD(APP_SUPERVISOR_TASK_STACK_SIZE))
    {
        /** Fail -> enter failt safe through mode manager */
        mode_man_enter_mode(MODE_MAN_FAIL_SAFE);
    }
    else
    {
        /** Stack ok */
    }
}