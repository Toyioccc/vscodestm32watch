/**
  ******************************************************************************
  * @file           : key.c
  * @brief          : Key initialization and management
  ******************************************************************************
  */

#include "key.h"

/* Key number variable */
uint8_t Key_Num;

/**
  * @brief  Key GPIO Initialization Function
  * @param  None
  * @retval None
  */
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* Configure GPIO pins : PB1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Configure GPIO pins : PA4 PA6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
  * @brief  Get the key number that was pressed
  * @param  None
  * @retval Key number (0 if no key pressed)
  */
uint8_t Key_GetNum(void)
{
	uint8_t temp;
	if(Key_Num)
	{
		temp = Key_Num;
		Key_Num = 0;
		return temp;
	}
	else
	{
		return 0;
	}
}

/**
  * @brief  Get the current key state
  * @param  None
  * @retval Key state (0-3)
  */
uint8_t Key_GetState(void)
{
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_SET)
	{
		return 1;
	}
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_SET)
	{
		return 2;
	}
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET)
	{
		return 3;
	}
	
	return 0;
}

/**
  * @brief  Key scanning tick function (should be called periodically)
  * @param  None
  * @retval None		
  */
void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t Currentstate, Prestate;
	Count++;
	if(Count >= 25)
	{
		Count = 0;
		Prestate = Currentstate;
		Currentstate = Key_GetState();
		if(Prestate != 0 && Currentstate==0)
		{
			Key_Num = Prestate;
		}
	}
}
