#include "os.h"
#include <string.h>

/** Definition of global variable */
QueueHandle_t g_adc_queue IPC_REGION;

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );
static void reverse(char str[], int32_t length);

/**
 * @brief Idle task memmory
 * 
 * @param ppxIdleTaskTCBBuffer Pointer point to a pointer to TCB task buffer
 * @param ppxIdleTaskStackBuffer Pointer point to a pointer to Stack buffer
 * @param pulIdleTaskStackSize Pointer point to Idle stack size
 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/** ISR handler */
void ADC_IRQHandler(void);

/** Static function */
static void SystemClock_Config(void);


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

inline void os_debug_put_ln(void)
{
  os_debug_put_string("\r\n");
}
/** Start adc sampling */
void adc_start_sampling(void)
{
  adc1_software_trigger();
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
  led_init();
  adc1_init();
  // iwdg_init();
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

static void reverse(char str[], int32_t length) {
    int32_t start = 0;
    int32_t end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char* os_itoa(int32_t num, char* str, int32_t base) {
    int32_t i = 0;
    int32_t isNegative = 0;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Handle negative numbers only if base is 10
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int32_t rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}