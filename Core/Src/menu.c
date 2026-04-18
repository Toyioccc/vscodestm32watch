#include "main.h"
#include "menu.h"
#include "MyRTC.h"
#include "OLED.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "settime.h"
#include "MPU6050.h"
#include "dino.h"
#include <math.h>

/* GB2312 bytes: "菜单" and "设置" */
#define CN_MENU_TEXT    "\xB2\xCB\xB5\xA5"
#define CN_SETTING_TEXT "\xC9\xE8\xD6\xC3"
#define CN_DINO_TEXT    "\xB9\xC8\xB8\xE8\xD0\xA1\xBF\xD6\xC1\xFA"

extern const uint8_t Diode[];

int clkflag=1;
int setflag=1;
void MPU6050_Caculation(void);
uint8_t KeyNum;

void Peripheral_Init(void)
{
	MyRTC_Init();
	KEY_Init();
	LED_Init();
}
/*----------------------------------首页时钟-------------------------------------*/
void Show_Clock_UI(void)
{
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
		KeyNum=Key_GetNum();

		if(KeyNum==1)
		{
			clkflag--;
			if(clkflag<=0)clkflag=2;
		}
		else if(KeyNum==2)
		{
			clkflag++;
			if(clkflag>=3)clkflag=1;
		}
		else if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			return clkflag;
		}

		switch(clkflag)
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
		}
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
		uint8_t setflag_temp=0;
		KeyNum=Key_GetNum();

		if(KeyNum==1)
		{
			setflag--;
			if(setflag<=0)setflag=2;
		}
		else if(KeyNum==2)
		{
			setflag++;
			if(setflag>=3)setflag=1;
		}
		else if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			setflag_temp=setflag;
		}

		if(setflag_temp==1){return ;}
		else if(setflag_temp==2){SetTime();}

		switch(setflag)
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
		}
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
		KeyNum=Key_GetNum();
		uint8_t menu_flag_temp=0;
		if(KeyNum==1)//上一项
		{
			DirectFlag=1;
			move_flag=1;
			menu_flag--;
			if(menu_flag<=0)menu_flag=7;
		}
		else if(KeyNum==2)//下一项
		{
			DirectFlag=2;
			move_flag=1;
			menu_flag++;
			if(menu_flag>=8)menu_flag=1;
		}
		else if(KeyNum==3)//确认
		{
			OLED_Clear();
			OLED_Update();
			menu_flag_temp=menu_flag;
		}
		
		if(menu_flag_temp==1){return 0;}
		else if(menu_flag_temp==2){MenuToFunction();StopWatch();}
		else if(menu_flag_temp==3){MenuToFunction();LED();}
	  	else if(menu_flag_temp==4){MenuToFunction();MPU6050();}
		else if(menu_flag_temp==5){MenuToFunction();Game();}
		else if(menu_flag_temp==6){MenuToFunction();Emoji();}
		else if(menu_flag_temp==7){MenuToFunction();Gradienter();}
			

			if(menu_flag==1)
			{
				if(DirectFlag==1)Set_Selection(move_flag,1,0);
				else if(DirectFlag==2)Set_Selection(move_flag,0,0);
			}
			
			else
			{
				if(DirectFlag==1)Set_Selection(move_flag,menu_flag,menu_flag-1);
				else if(DirectFlag==2)Set_Selection(move_flag,menu_flag-2,menu_flag-1);
			}
	}
}
/*----------------------------------秒表界面-------------------------------------*/
uint8_t hour=0;
uint8_t min=0;
uint8_t sec=0;
void Show_StopWatch_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
  OLED_Printf(32,20,OLED_8X16,"%02d:%02d:%02d",hour,min,sec);	
//	OLED_ShowString(8,44,"开始",OLED_8X16);
//	OLED_ShowString(48,44,"停止",OLED_8X16);
//	OLED_ShowString(88,44,"清除",OLED_8X16);
		OLED_ShowImage(32,42,16,16,Stopimage[0]);
		OLED_ShowImage(80,42,16,16,Stopimage[2]);
}
uint8_t start_timing_flag=0;//1 start 0 stop
uint8_t start_flag=0;
void StopWatch_Tick(void)
{
	static uint16_t Count;
	Count++;
	if(Count>=1000)
	{
		Count=0;
			if(start_timing_flag==1)
		{
			sec++;
			if(sec>=60)
			{
				sec=0;
				min++;
			}
			if(min>=60)
			{
				min=0;
				sec++;
			}
			if(hour>=99)
				hour=0;
		}
	}
	
}
uint8_t stopwatch_flag=1;

int StopWatch(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t stopwatchflag_temp=0;
		if(KeyNum==1)//上一项
		{
			stopwatch_flag--;
			start_flag=0;
			if(stopwatch_flag<=0)
				stopwatch_flag=3;
		}
		else if(KeyNum==2)//下一项
		{
			stopwatch_flag++;
			start_flag=0;
			if(stopwatch_flag>=4)
				stopwatch_flag=1;
		}
		else if(KeyNum==3)//确认
		{
			OLED_Clear();
			OLED_Update();
			stopwatchflag_temp=stopwatch_flag;
			start_flag++;
			if(start_flag>=3)
				start_flag=1;
		}
		if(stopwatchflag_temp==1)
		{return 0;}
		switch(stopwatch_flag)
		{
			case 1:
				Show_StopWatch_UI();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			case 2:
				Show_StopWatch_UI();
			  if(start_flag==1)
			  { 
					start_timing_flag=1;
					OLED_ShowImage(32,42,16,16,Stopimage[1]);
				}
				if(start_flag==2)
				{
					start_timing_flag=0;
					OLED_ShowImage(32,42,16,16,Stopimage[0]);
				}
				OLED_ReverseArea(32,42,16,16);
				OLED_Update();
				break;
			case 3:
				Show_StopWatch_UI();
				if(start_flag==1)
				{
					start_timing_flag=0;
					hour=min=sec=0;
				}
				OLED_ReverseArea(80,42,16,16);
				OLED_Update();
				break;
		}
	}
}
/*----------------------------------手电筒-------------------------------------*/
void Show_LED_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);	
	OLED_ShowString(20,20,"OFF",OLED_12X24);
	OLED_ShowString(72,20,"ON",OLED_12X24);
}
uint8_t led_flag=1;
int LED(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t led_flag_temp=0;
		if(KeyNum==1)//上一项
		{
			led_flag--;
			if(led_flag<=0)
				led_flag=3;
		}
		else if(KeyNum==2)//下一项
		{
			led_flag++;
			if(led_flag>=4)
				led_flag=1;
		}
		else if(KeyNum==3)//确认
		{
			OLED_Clear();
			OLED_Update();
			led_flag_temp=led_flag;
		}
		if(led_flag_temp==1)
		{return 0;}
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
		}
	}
}
/*----------------------------------mpu6050界面-------------------------------------*/
int16_t ax,ay,az,gx,gy,gz;//MPU6050测得的三轴加速度和角速度
float roll_g,pitch_g,yaw_g;//陀螺仪解算的欧拉角
float roll_a,pitch_a;//加速计解算的欧拉角
float roll,pitch,yaw;
float a=0.89  ;//互补滤波系数
float Delta_t=0.005;//采样周期
double pi=3.1415927;
static float gradienter_roll_zero = 0.0f;
static float gradienter_pitch_zero = 0.0f;
static uint8_t gradienter_calibrated = 0;
static uint8_t gradienter_zero_samples = 0;
static uint8_t mpu_inited_flag = 0;
#define GRADIENTER_CAL_SAMPLES 8U

static void Ensure_MPU6050_Inited(void)
{
	if (mpu_inited_flag == 0)
	{
		MPU6050_Init();
		mpu_inited_flag = 1;
	}
}

void MPU6050_Caculation(void)
{
	Delay_ms(5);
	MPU6050_GetData(&ax,&ay,&az,&gx,&gy,&gz);
	//通过陀螺仪解算欧拉角
	roll_g=roll+(float)gx*Delta_t/ 16.4f;;
	pitch_g=pitch+(float)gy*Delta_t/ 16.4f;;
	yaw_g=yaw+(float)gz*Delta_t/ 16.4f;;
	//通过加速度解算欧拉角
	pitch_a=atan2((-1)*ax,az)*180/pi;//atan(y,x)能根据坐标判断象限
	roll_a=atan2(ay,az)*180/pi;
	//通过互补滤波进行数据融合
	roll=a*roll_g+(1-a)*roll_a;
	pitch=a*pitch_g+(1-a)*pitch_a;
	yaw=a*yaw_g;
}
void Show_MPU6050_UI(void)
{
	int16_t roll10 = (int16_t)(roll * 10.0f);
	int16_t pitch10 = (int16_t)(pitch * 10.0f);
	int16_t yaw10 = (int16_t)(yaw * 10.0f);
	uint8_t roll_frac = (uint8_t)((roll10 < 0 ? -roll10 : roll10) % 10);
	uint8_t pitch_frac = (uint8_t)((pitch10 < 0 ? -pitch10 : pitch10) % 10);
	uint8_t yaw_frac = (uint8_t)((yaw10 < 0 ? -yaw10 : yaw10) % 10);

	OLED_ShowImage(0,0,16,16,Return);
	OLED_Printf(0,16,OLED_8X16,"R:%d.%1d", roll10 / 10, roll_frac);
	OLED_Printf(0,32,OLED_8X16,"P:%d.%1d", pitch10 / 10, pitch_frac);
	OLED_Printf(0,48,OLED_8X16,"Y:%d.%1d", yaw10 / 10, yaw_frac);
}
int MPU6050(void)
{
	uint8_t mpu_id;

	Ensure_MPU6050_Inited();

	OLED_Clear();
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(20,0,"MPU6050",OLED_8X16);
	OLED_Update();

	mpu_id = MPU6050_GetID();
	if (mpu_id != 0x68)
	{
		OLED_Clear();
		OLED_ShowImage(0,0,16,16,Return);
		OLED_ShowString(0,24,"MPU ERR",OLED_8X16);
		OLED_Printf(0,40,OLED_8X16,"ID:%02X",mpu_id);
		OLED_Update();
		while (1)
		{
			if (Key_GetNum() == 3)
			{
				OLED_Clear();
				OLED_Update();
				return 0;
			}
		}
	}

	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			return 0;
		}
		MPU6050_Caculation();
		OLED_Clear();
		Show_MPU6050_UI();
		OLED_ReverseArea(0,0,16,16);
		OLED_Update();
	}
}
/*----------------------------------恐龙小游戏界面-------------------------------------*/
void Show_Game_UI(void)
{
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,CN_DINO_TEXT,OLED_8X16);
}
uint8_t game_flag=1;
int Game(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t game_flag_temp=0;
		if(KeyNum==1)//上一项
		{
			game_flag--;
			if(game_flag<=0)
				game_flag=2;
		}
		else if(KeyNum==2)//下一项
		{
			game_flag++;
			if(game_flag>=3)
				game_flag=1;
		}
		else if(KeyNum==3)//确认
		{
			OLED_Clear();
			OLED_Update();
			game_flag_temp=game_flag;
		}
		if(game_flag_temp==1)
		{return 0;}
		else if(game_flag_temp==2)
		{
			DinoGame_Pos_Init();
			DinoGame_Animation();
		}
		switch(game_flag)
		{
			case 1:
				Show_Game_UI();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			case 2:
				Show_Game_UI();
			    LED1_OFF();
				OLED_ReverseArea(0,16,80,16);
				OLED_Update();
				break;
		}
	}
}
/*----------------------------------动态表情包-------------------------------------*/
void Show_Emoji_UI(void)
{
	for(uint8_t i=0;i<3;i++)
	{
		OLED_Clear();
		OLED_ShowImage(30,10+i,16,16,eyebrow[0]);
		OLED_ShowImage(82,10+i,16,16,eyebrow[1]);
		OLED_DrawEllipse(40,32,6,6-i,1);
		OLED_DrawEllipse(88,32,6,6-i,1);
		OLED_ShowImage(54,40,20,20,mouth);
		OLED_Update();
		Delay_ms(100);
	}
	for(uint8_t i=0;i<3;i++)
	{
		OLED_Clear();
		OLED_ShowImage(30,12-i,16,16,eyebrow[0]);
		OLED_ShowImage(82,12-i,16,16,eyebrow[1]);
		OLED_DrawEllipse(40,32,6,4+i,1);
		OLED_DrawEllipse(88,32,6,4+i,1);
		OLED_ShowImage(54,40,20,20,mouth);
		OLED_Update();
		Delay_ms(100);
	}
	Delay_ms(500);
}
int Emoji(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			return 0;
		}
		Show_Emoji_UI();
	}
}
/*----------------------------------水平仪-------------------------------------*/
void Show_Gradienter_UI(void)
{
	float roll_corr;
	float pitch_corr;
	int16_t ball_x;
	int16_t ball_y;

	MPU6050_Caculation();
	if (gradienter_zero_samples < GRADIENTER_CAL_SAMPLES)
	{
		gradienter_roll_zero = (gradienter_roll_zero * gradienter_zero_samples + roll_a) / (gradienter_zero_samples + 1);
		gradienter_pitch_zero = (gradienter_pitch_zero * gradienter_zero_samples + pitch_a) / (gradienter_zero_samples + 1);
		gradienter_zero_samples++;
		if (gradienter_zero_samples >= GRADIENTER_CAL_SAMPLES)
		{
			gradienter_calibrated = 1;
		}
	}
	roll_corr = roll_a - gradienter_roll_zero;
	pitch_corr = pitch_a - gradienter_pitch_zero;
	ball_x = 64 - (int16_t)(2.0f * roll_corr);
	ball_y = 32 + (int16_t)(2.0f * pitch_corr);

	if (ball_x < 34) ball_x = 34;
	if (ball_x > 94) ball_x = 94;
	if (ball_y < 2) ball_y = 2;
	if (ball_y > 62) ball_y = 62;

	OLED_DrawCircle(64,32,30,0);
	OLED_DrawCircle(ball_x,ball_y,4,1);
}
int Gradienter(void)
{
	Ensure_MPU6050_Inited();
	gradienter_calibrated = 0;
	gradienter_zero_samples = 0;
	gradienter_roll_zero = 0.0f;
	gradienter_pitch_zero = 0.0f;

	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			return 0;
		}
		OLED_Clear();
		Show_Gradienter_UI();
		OLED_Update();
	}
}
