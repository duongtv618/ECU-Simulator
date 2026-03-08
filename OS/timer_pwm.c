#include "timer_pwm.h"
#include "stm32f4xx.h"

/**
 * @brief Initialize timer pwm
 * Use timer 2 channel 3 pwm pin PB10
 * 
 */
void timer_pwm_init(void){
    /** Clock for gpiob and timer 2 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /** Set PB10 as alternative function */
    GPIOB->MODER &= ~GPIO_MODER_MODER10;
    GPIOB->MODER |= GPIO_MODER_MODER10_1;

    /** Connect AF1 to PB10 */
    GPIOB->AFR[1] &= ~(0xF << GPIO_AFRH_AFSEL10_Pos);//Clear
    GPIOB->AFR[1] |= (1 << GPIO_AFRH_AFSEL10_Pos);//Set AF1

    /** Time base */
    //100MHz -> timer 1KHz
    TIM2->PSC = 100 - 1;
    TIM2->ARR = 1000 - 1;

    /** Configure PWM mode 1 on channel 3 */
    TIM2->CCMR2 |= (TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC3PE;

    /** Set initial duty cycle 50% */
    TIM2->CCR3 = 500;

    /** Enalbe output on channel 3 */
    TIM2->CCER |= TIM_CCER_CC3E;

    /** Start the timer */
    timer_pwm_start();
}

/**
 * @brief Write to timer 
 * 
 * @param percent 0 - 100%
 */
void timer_pwm_write(uint32_t percent){
    /** For debug, remove when release */
    assert_param(percent > 100);
    //There is another way, the RANGE make sure percent is only within 100 but it can be a bug in runtime, 
    //run control logic, RANGE is define later
    //uint32 writeVal = RANGE(percent, 0, 100);
    //TIM2->CCR3 = writeVal * 10;
    /** It's safe to change CCR3 register value without entering critical section */
    TIM2->CCR3 = percent * 10;
}

/**
 * @brief Start timer2 and pwm at PB10
 * 
 */
void inline timer_pwm_start(void){
    TIM2->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Stop the channel, not entire timer 2
 * 
 */
void inline timer_pwm_stop(void){
    TIM2->CCER &= ~TIM_CCER_CC3E;
}