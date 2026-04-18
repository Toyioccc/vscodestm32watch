/**
  ******************************************************************************
  * @file           : led.c
  * @brief          : LED initialization and management
  ******************************************************************************
  */

#include "led.h"

/**
  * @brief  LED GPIO Initialization Function
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* Configure GPIO pins : PA1 PA2 */
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/* Set LEDs OFF (high level) */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
}

/**
  * @brief  Turn on LED1
  * @param  None
  * @retval None
  */
void LED1_ON(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
}

/**
  * @brief  Turn off LED1
  * @param  None
  * @retval None
  */
void LED1_OFF(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
}

/**
  * @brief  Toggle LED1
  * @param  None
  * @retval None
  */