
#include "main.h"
#include "app.h"

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  app_init();
  app_start();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
