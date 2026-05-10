#ifndef __APP_KEY_H
#define __APP_KEY_H

#ifdef __cplusplus//
extern "C" {
#endif

#include "main.h"

/* External variables */

#define KEY_EVENT_QUEUE_LEN 8U
#define KEY_CONFIRM  GPIO_PIN_4
#define KEY_UP GPIO_PIN_1
#define KEY_DOWN GPIO_PIN_6
/**
  * @brief  Key initialization function
  * @param  None
  * @retval None
  */
void KEY_Init(void);

/**
  * @brief  Get the key number that was pressed
  * @param  None
  * @retval Key number (0 if no key pressed)
  */
uint8_t Key_GetState(void);

/**
  * @brief  Key scanning tick function (should be called periodically)
  * @param  None
  * @retval None
  */
void Key_Tick(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_KEY_H */
