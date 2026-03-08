#include "jitter.h"
#include "os_cfg.h"

#include <stdint.h>
#include "stm32f4xx.h"

#ifndef ABS
#define ABS(x) (x = (x >= 0) ? x : -x)
#endif

extern void *volatile pxCurrentTCB;
struct jitter
{
    TaskHandle_t task;//Task handler
    uint32_t period;//Period
    uint32_t last;//Last CYCCNT
    int32_t max;//Max jitter
    int32_t min;//Min jitter
    uint32_t executedTime;//Executed time
    uint32_t maxExecTime;//Max executed time
    uint8_t first;//Ignore first messurement
} s_jitter[OS_JITTER_MAX_TASK] IPC_REGION;


/** Internal counter, need to zero manually */
IPC_REGION static uint8_t counter;

/**
 * @brief Initialize jitter
 * 
 */
void jitter_init(void)
{
  /* Enable the DWT counter */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;                     // Reset the cycle counter
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // Enable the cycle counter
  counter = 0;
}

/**
 * @brief Add task to monitor jitter
 * 
 * @param tsk Task to monitor
 * @param period Period
 */
void jitter_add_task(TaskHandle_t tsk, uint32_t period)
{
    if (counter >= OS_JITTER_MAX_TASK) return;

    s_jitter[counter].task = tsk;
    s_jitter[counter].period = period * (SystemCoreClock / 1000);
    s_jitter[counter].max = 0;
    s_jitter[counter].min = 0;
    s_jitter[counter].last = DWT->CYCCNT + s_jitter[counter].period;
    s_jitter[counter].first = 1;
    s_jitter[counter].executedTime = 0;
    s_jitter[counter].maxExecTime = 0;
    counter++;
}

/**
 * @brief task switch in definition
 * Because this function is call inside context switch
 * so no need to raise privilege
 * 
 */
void trace_task_switch_in(void)
{
    for (size_t i = 0; i < counter; i++)
    {
        if (s_jitter[i].task == pxCurrentTCB)
        {
            uint32_t now = DWT->CYCCNT;//Read cyccle counter
            int32_t diff = now - s_jitter[i].last;
            s_jitter[i].last = now;
            int32_t jitter = (int32_t)diff - (int32_t)s_jitter[i].period;
            if (s_jitter[i].first) {
                s_jitter[i].first = 0;
            } else {
                if (jitter > s_jitter[i].max){
                    s_jitter[i].max = jitter;
                } else if (jitter < s_jitter[i].min) {
                    s_jitter[i].min = jitter;
                }
            }
            break;
        }
    }
}

/**
 * @brief Taks switch out
 * For task executed time calculation
 * 
 */
void trace_task_switch_out(void)
{
    for (size_t i = 0; i < counter; i++)
    {
        if (s_jitter[i].task == pxCurrentTCB)
        {
            uint32_t now = DWT->CYCCNT;
            s_jitter[i].executedTime = now - s_jitter[i].last;
            if (s_jitter[i].executedTime > s_jitter[i].maxExecTime)
                s_jitter[i].maxExecTime = s_jitter[i].executedTime;
            break;
        }
    }
    
}

/**
 * @brief Get max jitter of task then clear max jitter of that task
 * 
 * @param task Task to get
 * @return uint32_t Return value
 */
uint32_t jitter_get_max_jitter(TaskHandle_t task)
{
    uint32_t ret = 0;
    for (size_t i = 0; i < counter; i++)
    {
        if (task == s_jitter[i].task)
        {
            ret = s_jitter[i].max;
            s_jitter[i].max = 0;
            break;
        }
    }
    return ret;
}

/**
 * @brief Get minimum jitter of task
 * 
 * @param task Task to get
 * @return int32_t Return value
 */
int32_t jitter_get_min_jitter(TaskHandle_t task){
    int32_t ret = 0;
    for (size_t i = 0; i < counter; i++){
        if (task == s_jitter[i].task){
            ret = s_jitter[i].min;
            s_jitter[i].min = 0;
            break;
        }
    }
    return ret;
}

/**
 * @brief Get max executed time
 * 
 * @param task Task to get
 * @return uint32_t Max executed time in uint32_t
 */
uint32_t jitter_get_max_exec_time(TaskHandle_t task){
    uint32_t ret = 0;
    for (size_t i = 0; i < counter; i++)
    {
        if (s_jitter[i].task == task)
        {
            ret = s_jitter[i].maxExecTime;
            s_jitter[i].maxExecTime = 0;
            break;
        }
    }
    return ret;
}