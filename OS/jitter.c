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
    s_jitter[counter].last = DWT->CYCCNT + s_jitter[counter].period;
    s_jitter[counter].first = 1;
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
        if (s_jitter->task == pxCurrentTCB)
        {
            uint32_t now = DWT->CYCCNT;//Read cyccle counter
            int32_t diff = now - s_jitter[i].last;
            s_jitter[i].last = now;
            int32_t jitter = (int32_t)diff - (int32_t)s_jitter[i].period;
            if (ABS(jitter) > s_jitter[i].max)
            {
                if (s_jitter[i].first)
                    s_jitter->first = 0;
                else
                    s_jitter[i].max = ABS(jitter);
            }
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