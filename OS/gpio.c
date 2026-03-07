
#include "gpio.h"
#include "stm32f4xx.h"

/**
 * @brief Initialize led builtin gpio
 * gpioa pin 5
 */
void led_init(void)
{
  /** Enable clock for gpioa */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /** PA5 out put, clear bit 10, 11 at pin 5 position */
  GPIOA->MODER &= ~GPIO_MODER_MODER5;

  /** Set bit 10 to select ouput mode */
  GPIOA->MODER |= GPIO_MODER_MODER5_0;
}
void led_toggle(void)
{
  GPIOA->ODR ^= GPIO_ODR_OD5;
}