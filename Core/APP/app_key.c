/**
  ******************************************************************************
  * @file           : key.c
  * @brief          : Key initialization and management
  ******************************************************************************
  */

#include "app_key.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
static osMessageQueueId_t g_KeyEventQueue;

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
	GPIO_InitStruct.Pin = KEY_UP;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Configure GPIO pins : PA4 PA6 */
	GPIO_InitStruct.Pin =  KEY_DOWN| KEY_CONFIRM;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	if (g_KeyEventQueue == NULL)
	{
		g_KeyEventQueue = osMessageQueueNew(KEY_EVENT_QUEUE_LEN, sizeof(uint8_t), NULL);
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
void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t Currentstate, Prestate;
	static uint32_t LastSampleMs;//上次采样时间
	uint32_t NowMs;

	NowMs = HAL_GetTick();
	if ((NowMs - LastSampleMs) < 2U)//采样间隔至少2ms
	{
		return;
	}
	LastSampleMs = NowMs;

	Count++;
	if(Count >= 5U)
	{
		Count = 0;
		Prestate = Currentstate;
		Currentstate = Key_GetState();
		/* Report key event on press edge for faster UI response. */
		if((Prestate == 0U) && (Currentstate != 0U))
		{
			if (g_KeyEventQueue != NULL)
			{
                (void)osMessageQueuePut(g_KeyEventQueue, &Currentstate, 0U, 0U);//重新放入事件队列以供UI线程使用
			}
		}
	}
    
}
