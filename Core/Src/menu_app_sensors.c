#include "main.h"
#include "menu.h"
#include "OLED.h"
#include "key.h"
#include "delay.h"
#include "MPU6050.h"
#include "cmsis_os.h"
#include <math.h>

#define GRADIENTER_CAL_SAMPLES 8U

static int16_t ax, ay, az, gx, gy, gz;
static float roll_g, pitch_g, yaw_g;
static float roll_a, pitch_a;
static float roll, pitch, yaw;
static float a = 0.89f;
static float delta_t = 0.005f;
static double pi = 3.1415927;
static float gradienter_roll_zero = 0.0f;
static float gradienter_pitch_zero = 0.0f;
static uint8_t gradienter_zero_samples = 0;
static uint8_t mpu_inited_flag = 0;
extern uint8_t KeyNum; //全局按键状态变量
static void Ensure_MPU6050_Inited(void)
{
	if (mpu_inited_flag == 0)
	{
		MPU6050_Init();
		mpu_inited_flag = 1;
	}
}

static void MPU6050_Caculation(void)
{
	Delay_ms(5);
	MPU6050_GetData(&ax,&ay,&az,&gx,&gy,&gz);
	roll_g = roll + (float)gx * delta_t / 16.4f;
	pitch_g = pitch + (float)gy * delta_t / 16.4f;
	yaw_g = yaw + (float)gz * delta_t / 16.4f;
	pitch_a = atan2((-1) * ax, az) * 180 / pi;
	roll_a = atan2(ay, az) * 180 / pi;
	roll = a * roll_g + (1 - a) * roll_a;
	pitch = a * pitch_g + (1 - a) * pitch_a;
	yaw = a * yaw_g;
}

static void Show_MPU6050_UI(void)
{
	int16_t roll10 = (int16_t)(roll * 10.0f);
	int16_t pitch10 = (int16_t)(pitch * 10.0f);
	int16_t yaw10 = (int16_t)(yaw * 10.0f);
	uint8_t roll_frac = (uint8_t)((roll10 < 0 ? -roll10 : roll10) % 10);
	uint8_t pitch_frac = (uint8_t)((pitch10 < 0 ? -pitch10 : pitch10) % 10);
	uint8_t yaw_frac = (uint8_t)((yaw10 < 0 ? -yaw10 : yaw10) % 10);

	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(20,0,"MPU ANGLE",OLED_8X16);
	OLED_Printf(0,18,OLED_8X16,"Y:%d.%1d", yaw10 / 10, yaw_frac);
	OLED_Printf(0,34,OLED_8X16,"P:%d.%1d", pitch10 / 10, pitch_frac);
	OLED_Printf(0,50,OLED_8X16,"R:%d.%1d", roll10 / 10, roll_frac);
}

int MPU6050(void)
{
	uint8_t mpu_id;
	uint8_t scl_level;
	uint8_t sda_level;

	OLED_Clear();
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(20,0,"MPU CHECK",OLED_8X16);
	OLED_ShowString(0,24,"CHECKING...",OLED_8X16);
	OLED_Update();
	osDelay(30);

	if (MPU6050_IsOnline() == 0U)
	{
		scl_level = (uint8_t)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
		sda_level = (uint8_t)HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
		OLED_Clear();
		OLED_ShowImage(0,0,16,16,Return);
		OLED_ShowString(0,24,"MPU OFFLINE",OLED_8X16);
		OLED_ShowString(0,40,"SCL",OLED_6X8);
		OLED_ShowString(36,40,scl_level ? "H" : "L",OLED_6X8);
		OLED_ShowString(56,40,"SDA",OLED_6X8);
		OLED_ShowString(92,40,sda_level ? "H" : "L",OLED_6X8);
		OLED_ShowString(0,52,"K3 BACK",OLED_6X8);
		OLED_Update();
		while (1)
		{
			if(KeyNum== 3)
			{
				OLED_Clear();
				OLED_Update();
				return 0;
			}
			osDelay(5);
		}
	}

	Ensure_MPU6050_Inited();
	osDelay(20);

	mpu_id = MPU6050_GetID();
	if (mpu_id != 0x68U)
	{
		OLED_Clear();
		OLED_ShowImage(0,0,16,16,Return);
		OLED_ShowString(0,24,"MPU ERR",OLED_8X16);
		OLED_ShowString(0,40,"ID",OLED_8X16);
		OLED_ShowHexNum(24,40,mpu_id,2,OLED_8X16);
		OLED_ShowString(0,52,"K3 BACK",OLED_6X8);
		OLED_Update();
		while (1)
		{
			if (KeyNum==3)
			{
				OLED_Clear();
				OLED_Update();
				return 0;
			}
			osDelay(5);
		}
	}

	while(1)
	{
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
		osDelay(20);
	}
}

static void Show_Gradienter_UI(void)
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
	gradienter_zero_samples = 0;
	gradienter_roll_zero = 0.0f;
	gradienter_pitch_zero = 0.0f;

	while(1)
	{
		if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			return 0;
		}
		OLED_Clear();
		Show_Gradienter_UI();
		OLED_Update();
		osDelay(5);
	}
}
