#ifndef TIMER_PWM_H
#define TIMER_PWM_H

#include <stdint.h>

void timer_pwm_init(void);
void timer_pwm_write(uint32_t percent);
void timer_pwm_start(void);
void timer_pwm_stop(void);

#endif