
#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

void adc1_init(void);
void adc1_software_trigger(void);
int32_t adc_poll_for_eoc(uint16_t timeOut);
uint16_t adc_get_value(void);

#endif /* __ADC_H__ */

