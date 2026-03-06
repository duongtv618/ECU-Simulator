#include "os_cfg.h"
#include "usart.h"

/** Internal varibale */
IPC_REGION static uint8_t usart2_dma_busy_flag;
IPC_REGION static uint8_t ring_buffer[USART_RING_BUFFER_SIZE];
IPC_REGION static uint16_t tx_head;
IPC_REGION static uint16_t tx_tail;
IPC_REGION static uint16_t tx_len;

extern uint32_t SystemCoreClock;

static void uart2_start_dma(void);
void DMA1_Stream6_IRQHandler(void);

/** DMA ISR */
void DMA1_Stream6_IRQHandler(void)
{
  if (DMA1->HISR & DMA_HISR_TCIF6)//Check flag
  {
    DMA1->HIFCR |= DMA_HIFCR_CTCIF6;//Clear it
    uint16_t sent = tx_len - DMA1_Stream6->NDTR;//Sent byte
    tx_tail += sent;//Now tail is updated
    if (tx_tail >= USART_RING_BUFFER_SIZE)//Then round. This is when ring buffer counter reach ring buffer size
      tx_tail = 0;
    usart2_dma_busy_flag = 0;//Clear flag for the next stream
    uart2_start_dma();//Then start again for the rest
  }
}

/** Write to tx dma usart2 */
void usart2_write_tx(uint8_t *dat, uint16_t len)
{
  for (size_t i = 0; i < len; i++)
  {
    ring_buffer[tx_head++] = dat[i];

    if (tx_head >= USART_RING_BUFFER_SIZE)
      tx_head = 0;
  }

  uart2_start_dma();
}

/**
 * @brief Start the dma, check head, tail, etc
 * 
 */
static void uart2_start_dma(void)
{
  /** Busy? */
  if (usart2_dma_busy_flag)
    return;

  /** Ring empty? */
  if (tx_head == tx_tail)
    return;

  /** Not empty then calculate length, break into to part if len > (tx_tail + USART_RING_BUFFER_SIZE - 1) */
  if (tx_head > tx_tail)
    tx_len = tx_head - tx_tail;
  else
    tx_len = USART_RING_BUFFER_SIZE - tx_tail;

  /** Set the dma stream */
  DMA1_Stream6->M0AR = (uint32_t)&ring_buffer[tx_tail];
  DMA1_Stream6->NDTR = tx_len;

  /** Raise busy flag */
  usart2_dma_busy_flag = 1;

  /** Start stream */
  DMA1_Stream6->CR |= DMA_SxCR_EN;
}

/**
 * @brief Initialize usart2 and dma unit
 * 
 */
void usart2_dma_init(void)
{
  /* Clock */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  /* PA2 = TX (AF7) */
  GPIOA->MODER &= ~(3 << (2 * 2));
  GPIOA->MODER |= (2 << (2 * 2)); // AF

  GPIOA->AFR[0] &= ~(0xF << (4 * 2));
  GPIOA->AFR[0] |= (7 << (4 * 2)); // AF7

  GPIOA->OSPEEDR |= (3 << (2 * 2)); // High speed

  /* Baud 1152000 */
  USART2->BRR = SystemCoreClock / 2 / 115200;

  USART2->CR1 = USART_CR1_TE;
  USART2->CR3 = USART_CR3_DMAT; // Enable DMA TX
  USART2->CR1 |= USART_CR1_UE;  // Enable UART

  /** Clock for dma */
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  /* Disable stream */
  DMA1_Stream6->CR &= ~DMA_SxCR_EN;
  while (DMA1_Stream6->CR & DMA_SxCR_EN)
    ;

  /* Peripheral address */
  DMA1_Stream6->PAR = (uint32_t)&USART2->DR;

  /* Configure */
  DMA1_Stream6->CR =
      (4 << 25)        // Channel 4
      | DMA_SxCR_MINC  // Memory increment
      | DMA_SxCR_DIR_0 // Memory to peripheral
      | DMA_SxCR_TCIE; // Transfer complete interrupt

  /* Enable IRQ */
  NVIC_SetPriority(DMA1_Stream6_IRQn, 5);
  NVIC_EnableIRQ(DMA1_Stream6_IRQn);

  /* IPC region is NOINIT so i have to initialze them */
  tx_head = 0;
  tx_tail = 0;
  usart2_dma_busy_flag = 0;
  tx_len = 0;
}
