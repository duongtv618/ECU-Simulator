#include "iwdg.h"
#include "app_cfg.h"
#include "stm32f4xx.h"

/**
 * @brief Initialize independent watchdog timer
 * 
 */
void iwdg_init(void){
  /** Trigger watchdog */
  IWDG->KR = IWDG_UNLOCK_CODE;

  /** Select prescaler 32 */
  IWDG->PR = 0x03;

  /** Reload value equal timeout milisecond value */
  IWDG->RLR = APP_WATCHDOG_TIMEOUT_MS;

  /** Wait till register is set */
  while (IWDG->SR != 0);
  
  /** First reload to load value to RLR register */
  iwdg_refresh();
}
void inline iwdg_refresh(void){
  IWDG->KR = IWDG_REFRESH_CODE;
}