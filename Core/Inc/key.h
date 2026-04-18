#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* External variables */
extern volatile uint8_t Key_Num;

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
uint8_t Key_GetNum(void);

/**
  * @brief  Get the current key state
  * @param  None
  * @retval Key state (0-3)
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

#endif /* __KEY_H */
