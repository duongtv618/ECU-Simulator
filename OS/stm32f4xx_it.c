/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "os.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
typedef struct log_fault_s
{
  uint32_t cfsr;
  uint32_t hfsr;
  uint32_t mmfar;
  uint32_t bfar;
  uint32_t pc;
  uint32_t lr;
  uint32_t xPSR;
  uint32_t r12;
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
} fault_log_t;

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
const char log_str_r0[] = "R0:\t0x";
const char log_str_r1[] = "R1:\t0x";
const char log_str_r2[] = "R2:\t0x";
const char log_str_r3[] = "R3:\t0x";
const char log_str_r12[] = "R12:\t0x";
const char log_str_lr[] = "LR:\t0x";
const char log_str_pc[] = "PC:\t0x";
const char log_str_xPSR[] = "xPSR:\t0x";
const char log_str_cfsr[] = "CFSR:\t0x";
const char log_str_hfsr[] = "HFSR:\t0x";
const char log_str_mmfar[] = "MMFAR:\t0x";
const char log_str_bfar[] = "BFAR:\t0x";

static fault_log_t s_fault;
static char log_msg_buf[20];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void hard_fault_handler(uint32_t *stack);
static void uart2_write_polling(const char *buf, uint16_t len);
static void inline write_ln(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void inline write_ln(void)
{
  uart2_write_polling("\r\n", strlen("\r\n"));
}

/**
 * @brief Write data to uart2 with polling technique, DMA can be broken now
 * 
 * @param buf Buffer to write
 * @param len Length of buffer
 */
static void uart2_write_polling(const char *buf, uint16_t len)
{
  for (int i = 0; i < len; i++)
  {
    while (!(USART2->SR & USART_SR_TXE))
      ;
    USART2->DR = buf[i];
  }
}

/**
 * @brief Hardfault handler with current stack address
 * 
 * @param stack Stack address
 */
static void hard_fault_handler(uint32_t *stack)
{
  /** Take stack frame */
  s_fault.r0 = stack[0];
  s_fault.r1 = stack[1];
  s_fault.r2 = stack[2];
  s_fault.r3 = stack[3];
  s_fault.r12 = stack[4];
  s_fault.lr = stack[5];
  s_fault.pc = stack[6];
  s_fault.xPSR = stack[7];

  /** Take control function register */
  s_fault.cfsr = SCB->CFSR;
  s_fault.hfsr = SCB->HFSR;
  s_fault.mmfar = SCB->MMFAR;
  s_fault.bfar = SCB->BFAR;

  /** Log to uart */
  uart2_write_polling(log_str_r0, strlen(log_str_r0));
  uart2_write_polling(os_itoa(s_fault.r0, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_r1, strlen(log_str_r1));
  uart2_write_polling(os_itoa(s_fault.r1, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_r2, strlen(log_str_r2));
  uart2_write_polling(os_itoa(s_fault.r2, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_r12, strlen(log_str_r12));
  uart2_write_polling(os_itoa(s_fault.r12, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_lr, strlen(log_str_lr));
  uart2_write_polling(os_itoa(s_fault.lr, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_pc, strlen(log_str_pc));
  uart2_write_polling(os_itoa(s_fault.pc, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_xPSR, strlen(log_str_xPSR));
  uart2_write_polling(os_itoa(s_fault.xPSR, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_cfsr, strlen(log_str_cfsr));
  uart2_write_polling(os_itoa(s_fault.cfsr, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_hfsr, strlen(log_str_hfsr));
  uart2_write_polling(os_itoa(s_fault.hfsr, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_mmfar, strlen(log_str_mmfar));
  uart2_write_polling(os_itoa(s_fault.mmfar, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();
  uart2_write_polling(log_str_bfar, strlen(log_str_bfar));
  uart2_write_polling(os_itoa(s_fault.bfar, log_msg_buf, 16), strlen(log_msg_buf));
  write_ln();

  /** Keep in loop so that IWDG can reset MCU */
  while (1);
}
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}




/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  uint32_t *stack;
  /** Choose current stack pointer to store stack frame */
  if (__get_CONTROL() & 2)
    stack = (uint32_t *)__get_PSP();
  else
    stack = (uint32_t *)__get_MSP();
  hard_fault_handler(stack);
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
  volatile uint32_t fault_address = SCB->MMFAR;
  volatile uint32_t cfsr = SCB->CFSR;
  volatile uint32_t mmfsr = (cfsr & 0xFF);
  volatile uint32_t fault_status = (mmfsr & 0x7F);
  (void)fault_address;
  (void)cfsr;
  (void)mmfsr;
  (void)fault_status;
  /** Can log to FLash or RTC register, in Mem Fault unprivileged task has just
   * access to an inaccessable region, can use msp to work with log function
   */
  os_log(OS_DEBUG_CONSOLE, (uint8_t*)"Have memmanage fault\r\n", 0);
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /** Then reset ECU */
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/
/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
