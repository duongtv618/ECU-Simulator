#include "os.h"
#include <string.h>

/** Definition of global variable */
QueueHandle_t g_adc_queue IPC_REGION;

/** ISR handler */
void ADC_IRQHandler(void);

/** Static function */
static void SystemClock_Config(void);

/** ADC interrupt handler */
void ADC_IRQHandler(void)
{
  if (ADC1->SR & ADC_SR_EOC)
  {
    uint16_t val = ADC1->DR;
    BaseType_t xwake = pdFALSE;
    xQueueSendFromISR(g_adc_queue, &val, &xwake);
    portYIELD_FROM_ISR(xwake);
  }
}

/** Register jitter task from app */
void os_registerJitterTask(TaskHandle_t tskHandler, uint16_t periodMS)
{
  if (tskHandler == NULL)
    return;

  jitter_add_task(tskHandler, periodMS);
}

/**
 * @brief Get max jitter in us  
 * 
 * @param task Task to get
 * @return uint32_t Return value in us
 */
uint32_t os_getMaxJitter(TaskHandle_t task)
{
  return jitter_get_max_jitter(task);
}

/**
 * @brief Put String to debug console, currentlt USART2
 * 
 * @param str pointer to string source
 */
void os_debug_put_string(const char *str)
{
  if (portIS_PRIVILEGED() == pdFALSE)
    portRAISE_PRIVILEGE();
  usart2_write_tx((uint8_t *)str, strlen(str));
  portRESET_PRIVILEGE();
}

/** Start adc sampling */
void adc_start_sampling(void)
{
}

/** Stop adc sampling */
void adc_stop_sampling(void)
{
}
/** Initialize os */
void os_init(void)
{
  HAL_Init();           // HAL first
  SystemClock_Config(); // Then system PLL

  /** Then peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  // MX_IWDG_Init();
  MX_TIM3_Init();
  jitter_init();

  usart2_dma_init();
}

/** System clock config */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

