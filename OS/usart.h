#ifndef USART_H
#define USART_H

#include "stm32f4xx.h"


void usart2_dma_init(void);
void usart2_write_tx(uint8_t* dat, uint16_t len);

#endif