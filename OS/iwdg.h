#ifndef __IWDG_H__
#define __IWDG_H__

#define IWDG_UNLOCK_CODE 0xCCCC
#define IWDG_REFRESH_CODE 0xAAAA

void iwdg_init(void);
void iwdg_refresh(void);

#endif /* __IWDG_H__ */

