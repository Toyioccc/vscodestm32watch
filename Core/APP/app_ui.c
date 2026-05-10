#include "main.h"
#include "menu.h"
#include "cmsis_os.h"
#include "MyRTC.h"
#include "OLED.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "settime.h"
#include "MPU6050.h"
#include "dino.h"
#include <math.h>
#include "AD.h"
/* GB2312 bytes: "菜单" and "设置" */
#define CN_MENU_TEXT    "\xB2\xCB\xB5\xA5"
#define CN_SETTING_TEXT "\xC9\xE8\xD6\xC3"
#define CN_DINO_TEXT    "\xB9\xC8\xB8\xE8\xD0\xA1\xBF\xD6\xC1\xFA"

extern const uint8_t Diode[];

int clkflag=1;
int setflag=1;
void MPU6050_Caculation(void);
uint8_t KeyNum;
void Show_Clock_UI(void);
void Show_SettingPage_UI(void);
void MenuToFunction(void);
void Set_Selection(uint8_t start_move,uint8_t Pre_Selection,uint8_t Target_Selection);
extern uint8_t move_flag;
extern uint8_t menu_flag;
volatile uint16_t ADValue;
float VBAT;
int Battery_Capacity;

void Battery_ReadADC(void)
{
	uint32_t sum = 0;
	for (int i = 0; i < 100; i++)
	{
		sum += AD_GetValue();
		osDelay(1);
	}
	ADValue = sum / 100;
	VBAT = (float)ADValue / 4095 * 3.3f;
	Battery_Capacity = (ADValue - 3276) * 100 / 4095;
	if (Battery_Capacity < 0)
		Battery_Capacity = 0;
	if (Battery_Capacity > 100)
		Battery_Capacity = 100;
}

void Show_Battery(void)
{
	OLED_ShowNum(85, 4, Battery_Capacity, 3, OLED_6X8);
	OLED_ShowChar(103, 4, '%', OLED_6X8);
	OLED_ShowNum(64,0,ADValue,4,OLED_6X8);
	if (Battery_Capacity == 100)
		OLED_ShowImage(110, 0, 16, 16, Battery);
	else if (Battery_Capacity >= 10 && Battery_Capacity < 100)
	{
		OLED_ShowImage(110, 0, 16, 16, Battery);
		OLED_ClearArea((112 + Battery_Capacity / 10), 5, (10 - Battery_Capacity / 10), 6);
		OLED_ClearArea(85, 4, 6, 8);
	}
	else
	{
		OLED_ShowImage(110, 0, 16, 16, Battery);
		OLED_ClearArea(112, 5, 10, 6);
		OLED_ClearArea(85, 4, 12, 8);
	}
}
static void ClockPage_HandleNavigation(uint8_t key_num)
{
	if (key_num == 1)
	{
		clkflag--;
		if (clkflag <= 0)
		{
			clkflag = 2;
		}
	}
	else if (key_num == 2)
	{
		clkflag++;
		if (clkflag >= 3)
		{
			clkflag = 1;
		}
	}
}

static void ClockPage_Render(void)
{
	switch (clkflag)
	{
		case 1:
			Show_Clock_UI();
			OLED_ReverseArea(0,48,32,16);
			OLED_Update();
			break;

		case 2:
			Show_Clock_UI();
			OLED_ReverseArea(96,48,32,16);
			OLED_Update();
			break;

		default:
			break;
	}
}

static void SettingPage_HandleNavigation(uint8_t key_num)
{
	if (key_num == 1)
	{
		setflag--;
		if (setflag <= 0)
		{
			setflag = 2;
		}
	}
	else if (key_num == 2)
	{
		setflag++;
		if (setflag >= 3)
		{
			setflag = 1;
		}
	}
}

static void SettingPage_Render(void)
{
	switch (setflag)
	{
		case 1:
			Show_SettingPage_UI();
			OLED_ReverseArea(0,0,16,16);
			OLED_Update();
			break;

		case 2:
			Show_SettingPage_UI();
			OLED_ReverseArea(0,16,96,16);
			OLED_Update();
			break;

		default:
			break;
	}
}

static void Menu_HandleNavigation(uint8_t key_num, uint8_t *direct_flag)
{
	if (key_num == 1)
	{
		*direct_flag = 1;
		move_flag = 1;
		menu_flag--;
		if (menu_flag <= 0)
		{
			menu_flag = 7;
		}
	}
	else if (key_num == 2)
	{
		*direct_flag = 2;
		move_flag = 1;
		menu_flag++;
		if (menu_flag >= 8)
		{
			menu_flag = 1;
		}
	}
}

static uint8_t Menu_DispatchAction(uint8_t action)
{
	switch (action)
	{
		case 1:
			return 1;

		case 2:
			MenuToFunction();
			StopWatch();
			break;

		case 3:
			MenuToFunction();
			LED();
			break;

		case 4:
			MenuToFunction();
			MPU6050();
			break;

		case 5:
			MenuToFunction();
			Game();
			break;

		case 6:
			MenuToFunction();
			Emoji();
			break;

		case 7:
			MenuToFunction();
			Gradienter();
			break;

		default:
			break;
	}

	return 0;
}

static void Menu_RenderCurrentSelection(uint8_t direct_flag)
{
	if (menu_flag == 1)
	{
		if (direct_flag == 1)
		{
			Set_Selection(move_flag,1,0);
		}
		else
		{
			Set_Selection(move_flag,0,0);
		}
	}
	else
	{
		if (direct_flag == 1)
		{
			Set_Selection(move_flag,menu_flag,menu_flag-1);
		}
		else
		{
			Set_Selection(move_flag,menu_flag-2,menu_flag-1);
		}
	}
}

void Peripheral_Init(void)
{
	MyRTC_Init();
	KEY_Init();
	LED_Init();
}
/*----------------------------------首页时钟-------------------------------------*/
void Show_Clock_UI(void)
{
	Show_Battery();
	MyRTC_ReadTime();
	OLED_Printf(0,0,OLED_6X8,"%04d-%02d-%02d",MyRTC_Time[0],MyRTC_Time[1],MyRTC_Time[2]);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",MyRTC_Time[3],MyRTC_Time[4],MyRTC_Time[5]);
	OLED_ShowString(0,48,CN_MENU_TEXT,OLED_8X16);
	OLED_ShowString(96,48,CN_SETTING_TEXT,OLED_8X16);
}

/* Keep compatibility with existing declaration in menu.h */
int First_Page_Clock(void)
{
	while(1)
	{
		ClockPage_HandleNavigation(KeyNum);

		if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			return clkflag;
		}

		ClockPage_Render();
	}
}
/*----------------------------------设置界面-------------------------------------*/
void Show_SettingPage_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"\xC8\xD5\xC6\xDA\xCA\xB1\xBC\xE4\xC9\xE8\xD6\xC3",OLED_8X16);
}

// void Menu(void)
// {
// 	OLED_Clear();
// 	OLED_ShowString(0, 0, "Menu", OLED_8X16);
// 	OLED_ShowString(0, 24, "Press KEY3 back", OLED_6X8);
// 	OLED_Update();

// 	while (1)
// 	{
// 		if (Key_GetNum() == 3)
// 		{
// 			OLED_Clear();
// 			OLED_Update();
// 			break;
// 		}
// 	}
// }

void SettingPage(void)
{
	while(1)
	{
		SettingPage_HandleNavigation(KeyNum);

		if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			if(setflag==1)
			{
				return;
			}
			if(setflag==2)
			{
				SetTime();
			}
		}

		SettingPage_Render();
	}
}
/*----------------------------------滑动菜单界面-------------------------------------*/
uint8_t pre_selection;//上次选择的选项
uint8_t target_selection;//目标选项
uint8_t x_pre=48;//上次选项的x坐标
uint8_t Speed=8;//速度
uint8_t move_flag;//开始移动的标志位，1表示开始移动，0表示停止移动
static uint32_t menu_anim_last_tick = 0;
#define MENU_ANIM_STEP_MS 10U
#define MENU_GRAPH_COUNT 8U
#define MENU_TO_FUNC_FRAMES 2U
#define MENU_TO_FUNC_STEP_Y 24U

static uint8_t Menu_Graph_WrapIndex(int16_t index)
{
	while (index < 0)
	{
		index += MENU_GRAPH_COUNT;
	}
	while (index >= MENU_GRAPH_COUNT)
	{
		index -= MENU_GRAPH_COUNT;
	}
	return (uint8_t)index;
}

void Menu_Animation(void)
{
	OLED_Clear();
	OLED_ShowImage(42,10,44,44,Frame);
	
	if (move_flag)
	{
		uint32_t now = HAL_GetTick();
		if ((now - menu_anim_last_tick) >= MENU_ANIM_STEP_MS)
		{
			menu_anim_last_tick = now;

			if (pre_selection < target_selection)
			{
				int16_t next_x = (int16_t)x_pre - Speed;
				if (next_x <= 0)
				{
					pre_selection++;
					move_flag = 0;
					x_pre = 48;
				}
				else
				{
					x_pre = (uint8_t)next_x;
				}
			}
			else if (pre_selection > target_selection)
			{
				int16_t next_x = (int16_t)x_pre + Speed;
				if (next_x >= 96)
				{
					pre_selection--;
					move_flag = 0;
					x_pre = 48;
				}
				else
				{
					x_pre = (uint8_t)next_x;
				}
			}
		}
	}
	
	OLED_ShowImage(x_pre-96,16,32,32,Menu_Graph[Menu_Graph_WrapIndex((int16_t)pre_selection - 2)]);
	OLED_ShowImage(x_pre-48,16,32,32,Menu_Graph[Menu_Graph_WrapIndex((int16_t)pre_selection - 1)]);
	OLED_ShowImage(x_pre,16,32,32,Menu_Graph[Menu_Graph_WrapIndex(pre_selection)]);
	OLED_ShowImage(x_pre+48,16,32,32,Menu_Graph[Menu_Graph_WrapIndex((int16_t)pre_selection + 1)]);
	OLED_ShowImage(x_pre+96,16,32,32,Menu_Graph[Menu_Graph_WrapIndex((int16_t)pre_selection + 2)]);
	
	OLED_Update();
}

void Set_Selection(uint8_t start_move,uint8_t Pre_Selection,uint8_t Target_Selection)
{
	if(start_move==1)
	{
		if ((move_flag == 0) || (pre_selection != Pre_Selection) || (target_selection != Target_Selection))
		{
			pre_selection=Pre_Selection;
			target_selection=Target_Selection;
			move_flag = 1;
			menu_anim_last_tick = HAL_GetTick();
		}
		
	}
	Menu_Animation();
}

void MenuToFunction(void)
{
	for(uint8_t i=0;i<MENU_TO_FUNC_FRAMES;i++)
	{
		OLED_Clear();
		OLED_ShowImage(x_pre-48,16+MENU_TO_FUNC_STEP_Y*i,32,32,Menu_Graph[Menu_Graph_WrapIndex((int16_t)pre_selection - 1)]);
		OLED_ShowImage(x_pre,16+MENU_TO_FUNC_STEP_Y*i,32,32,Menu_Graph[Menu_Graph_WrapIndex(pre_selection)]);
		OLED_ShowImage(x_pre+48,16+MENU_TO_FUNC_STEP_Y*i,32,32,Menu_Graph[Menu_Graph_WrapIndex((int16_t)pre_selection + 1)]);
		
		OLED_Update();
	}

	/* Ensure transition frames don't remain on screen when entering next page. */
	OLED_Clear();
	OLED_Update();
	
}


uint8_t menu_flag=1;
int Menu(void)
{
	move_flag=1;
	uint8_t DirectFlag=2;//置1：移动到上一项；置2：移动到下一项
	while(1)
	{
		Menu_HandleNavigation(KeyNum, &DirectFlag);

		if(KeyNum==3)//确认
		{
			OLED_Clear();
			OLED_Update();

			if (Menu_DispatchAction(menu_flag))
			{
				return 0;
			}
		}

		Menu_RenderCurrentSelection(DirectFlag);
	}
}
