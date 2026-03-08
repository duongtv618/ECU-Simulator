#include "app_cfg.h"

#include "adc.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "queue.h"

#define TS_CAL1 0x1FFF7A2C
#define TS_CAL2 0x1FFF7A2E

extern QueueHandle_t g_adc_queue;

void ADC_IRQHandler(void);

/**
 * @brief Initialize the ADC1
 *
 */
void adc1_init(void)
{
  /** Clock enable */
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  /** Divide by 4 cause of main clock is 100MHz */
  ADC->CCR &= ~ADC_CCR_ADCPRE;
  ADC->CCR |= (1 << ADC_CCR_ADCPRE_Pos);

  /** Use internal temperature sensor */
  ADC->CCR |= ADC_CCR_TSVREFE;

  /** Set up sampling time */
  ADC1->SMPR1 |= (7 << ADC_SMPR1_SMP18_Pos);

  /** Select channel 18 for temperature sensor */
  ADC1->SQR3 = 18;

  /** Enable end of conversion interrupt */
  // ADC1->CR1 |= ADC_CR1_EOCIE;

  /** Enable ADC1 */
  ADC1->CR2 |= ADC_CR2_ADON;

  /** Enable interrupt */
  // NVIC_SetPriority(ADC_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY - 1);
  // NVIC_EnableIRQ(ADC_IRQn);
}

/**
 * @brief Software trigger adc1
 *
 */
void adc1_software_trigger(void)
{
  ADC1->CR2 |= ADC_CR2_SWSTART;
}

/** ADC interrupt handler */
void ADC_IRQHandler(void)
{
  if (ADC1->SR & ADC_SR_EOC)
  {
    uint16_t val = ADC1->DR;
    BaseType_t xwake = pdFALSE;
#ifdef INJECT_LONG_INTERRUPT_HANDLE
    for (size_t i = 0; i < 60000; i++)
      ;
#endif
    xQueueSendFromISR(g_adc_queue, &val, &xwake);
    portYIELD_FROM_ISR(xwake);
  }
}

/**
 * @brief Poll for eoc
 * 
 * @param timeOut Time to wait
 * @return int32_t -1 if error, 0 if no error return
 */
int32_t adc_poll_for_eoc(uint16_t timeOut){
  while (!(ADC1->SR & ADC_SR_EOC) && timeOut--);
  if (timeOut == 0)
    return -1;
  else
    return 0;
}


uint16_t adc_get_value(void){
  return ADC1->DR;
}