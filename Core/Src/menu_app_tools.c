#include "main.h"
#include "menu.h"
#include "OLED.h"
#include "key.h"
#include "led.h"
#include "cmsis_os.h"
extern uint8_t KeyNum;
extern osMessageQueueId_t g_KeyEventQueue;
static uint8_t hour = 0;
static uint8_t min = 0;
static uint8_t sec = 0;
static uint8_t start_timing_flag = 0;
static uint8_t start_flag = 0;
static uint8_t stopwatch_flag = 1;
static uint8_t led_flag = 1;

static void Show_StopWatch_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_Printf(32,20,OLED_8X16,"%02d:%02d:%02d",hour,min,sec);
	OLED_ShowImage(32,42,16,16,Stopimage[0]);
	OLED_ShowImage(80,42,16,16,Stopimage[2]);
}

void StopWatch_Tick(void)
{
	static uint32_t last_tick_ms = 0;
	static uint32_t acc_ms = 0;
	uint32_t now_ms = HAL_GetTick();
	uint32_t delta_ms;

	if (last_tick_ms == 0U)
	{
		last_tick_ms = now_ms;
		return;
	}

	delta_ms = now_ms - last_tick_ms;
	if (delta_ms == 0U)
	{
		return;
	}
	last_tick_ms = now_ms;

	if (start_timing_flag != 1U)
	{
		return;
	}

	acc_ms += delta_ms;
	while (acc_ms >= 1000U)
	{
		acc_ms -= 1000U;
		sec++;
		if (sec >= 60U)
		{
			sec = 0U;
			min++;
		}
		if (min >= 60U)
		{
			min = 0U;
			hour++;
		}
		if (hour >= 99U)
		{
			hour = 0U;
		}
	}
}

int StopWatch(void)
{
	uint8_t action;
	uint8_t key;

	while (1)
	{
		/* Read fresh key event from queue (non-blocking), clear stale KeyNum */
		key = 0;
		if (osMessageQueueGet(g_KeyEventQueue, &key, NULL, 0) == osOK)
			KeyNum = key;
		else
			KeyNum = 0;

		action = 0;
		StopWatch_Tick();
		if (KeyNum == 1)
		{
			stopwatch_flag--;
			start_flag = 0;
			if (stopwatch_flag <= 0)
			    stopwatch_flag = 3;
		}
		else if(KeyNum == 2)
		{
			stopwatch_flag++;
			start_flag = 0;
			if (stopwatch_flag >= 4)
				stopwatch_flag = 1;
		}
		else if (KeyNum == 3)
		{
			OLED_Clear();
			OLED_Update();
			action = stopwatch_flag;
			start_flag++;
			if (start_flag >= 3)
				start_flag = 1;
		}

		if (action == 1)
		{
			start_timing_flag = 0;
			start_flag = 0;
			return 0;  /* back to menu */
		}

		switch (stopwatch_flag)
		{
			case 1:
				Show_StopWatch_UI();
				OLED_ReverseArea(0, 0, 16, 16);
				OLED_Update();
				break;
			case 2:
				Show_StopWatch_UI();
				if (start_flag == 1)
				{
					start_timing_flag = 1;
					OLED_ShowImage(32, 42, 16, 16, Stopimage[1]);
				}
				if (start_flag == 2)
				{
					start_timing_flag = 0;
					OLED_ShowImage(32, 42, 16, 16, Stopimage[0]);
				}
				OLED_ReverseArea(32, 42, 16, 16);
				OLED_Update();
				break;
			case 3:
				Show_StopWatch_UI();
				if (start_flag == 1)
				{
					start_timing_flag = 0;
					hour = min = sec = 0;
				}
				OLED_ReverseArea(80, 42, 16, 16);
				OLED_Update();
				break;
		}

		osDelay(20);  /* yield to scheduler, 50fps UI refresh */
	}
}

static void Show_LED_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(20,20,"OFF",OLED_12X24);
	OLED_ShowString(72,20,"ON",OLED_12X24);
}

int LED(void)
{
	while(1)
	{
		uint8_t key_num = KeyNum;
		uint8_t action = 0;
		if(key_num==1)
		{
			led_flag--;
			if(led_flag<=0)
			{
				led_flag=3;
			}
		}
		else if(key_num==2)
		{
			led_flag++;
			if(led_flag>=4)
			{
				led_flag=1;
			}
		}
		else if(key_num==3)
		{
			OLED_Clear();
			OLED_Update();
			action=led_flag;
		}
		if(action==1)
		{
			return 0;
		}
		switch(led_flag)
		{
			case 1:
				Show_LED_UI();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			case 2:
				Show_LED_UI();
				LED1_OFF();
				OLED_ReverseArea(20,20,36,24);
				OLED_Update();
				break;
			case 3:
				Show_LED_UI();
				LED1_ON();
				OLED_ReverseArea(72,20,24,24);
				OLED_Update();
				break;
			default:
				break;
		}
	}
}
