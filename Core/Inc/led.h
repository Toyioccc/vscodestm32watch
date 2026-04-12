#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
  * @brief  LED initialization function
  * @param  None
  * @retval None
  */
void LED_Init(void);

/**
  * @brief  Turn on LED1
  * @param  None
  * @retval None
  */
void LED1_ON(void);

/**
  * @brief  Turn off LED1
  * @param  None
  * @retval None
  */
void LED1_OFF(void);

/**
  * @brief  Toggle LED1
  * @param  None
  * @retval None
  */
void LED1_Turn(void);

/**
  * @brief  Turn on LED2
  * @param  None
  * @retval None
  */
#ifdef __cplusplus
}
#endif

#endif /* __LED_H */
