#include"main.h"
#include "dino.h"
#include "OLED.h"
#include "key.h"
#include "delay.h"
#include <math.h>
#include <stdlib.h>

#define DINO_GROUND_STEP_MS   14U
#define DINO_CLOUD_STEP_MS    32U
#define DINO_SCORE_STEP_MS   120U
#define DINO_JUMP_HEIGHT_PX   24.0f
#define DINO_JUMP_TIME_MS    700U

int Score;
struct Object_Position{
	uint8_t minX,minY,maxX,maxY;
};
void Show_Score(void)
{
	OLED_ShowNum(98,0,Score,5,OLED_6X8);
	
}
uint16_t Ground_Pos;
void Show_Ground(void)
{
	if(Ground_Pos<128)//此时显示前128个，i必须小于128，否则越界
	{
			for(uint8_t i=0;i<128;i++)
		{
			OLED_DisplayBuf[7][i]=Ground[i+Ground_Pos];//图像未滑出末尾，直接正向截取。for循环时间小于Ground_Pos自增
		}
	}
	else//图像滑出末尾，分两段循环截取
	{
			for(uint8_t i=0;i<255-Ground_Pos;i++)//取剩余字节
		{
			OLED_DisplayBuf[7][i]=Ground[i+Ground_Pos];
		}
		for(uint8_t i=255-Ground_Pos;i<128;i++)//填充显示
		{
			OLED_DisplayBuf[7][i]=Ground[i-(255-Ground_Pos)];
		}
	}
}
uint8_t barrier_flag;
uint8_t barrier_pos;
uint8_t Cloud_Pos;
uint8_t Jump_Pos;
extern double pi;
struct Object_Position Barrier;

static uint32_t g_dino_last_tick_ms;
static uint32_t g_score_acc_ms;
static uint32_t g_ground_acc_ms;
static uint32_t g_cloud_acc_ms;
static uint32_t g_jump_acc_ms;

static uint8_t Dino_NextBarrierType(uint8_t prev)
{
	uint8_t next = rand() % 3;
	if (next == prev)
	{
		next = (next + 1 + (rand() % 2)) % 3;
	}
	return next;
}

void Show_Barrier(void)
{
	OLED_ShowImage(127-barrier_pos,44,16,18,barrier[barrier_flag]);//实现随机出现障碍物并且障碍物像左移，随着barrierpos增大，x坐标减小
	/* Shrink hitbox a little to match visible solid pixels better. */
	Barrier.minX=127-barrier_pos+2;
	Barrier.maxX=143-barrier_pos-2;
	Barrier.minY=44+2;
	Barrier.maxY=62-1;
}
void Show_Cloud(void)
{
	OLED_ShowImage(127-Cloud_Pos,9,16,8,Cloud);
}
uint8_t jump_flag=0;
extern uint8_t KeyNum;
uint16_t jump_t;
struct Object_Position dino;
void Show_Dino(void)
{
	float jump_phase;

	KeyNum=Key_GetNum();
	if(KeyNum==1)
		jump_flag=1;

	if (jump_flag == 1U)
	{
		jump_phase = (float)jump_t / (float)DINO_JUMP_TIME_MS;
		if (jump_phase > 1.0f)
		{
			jump_phase = 1.0f;
		}
		Jump_Pos=(uint8_t)(DINO_JUMP_HEIGHT_PX * sin((float)(pi * jump_phase)));
	}
	else
	{
		Jump_Pos = 0U;
	}

	if(jump_flag==0)
	{
		if(Cloud_Pos%2==0)
		{
			OLED_ShowImage(0,44,16,18,Dino[0]);
		}
		else
			OLED_ShowImage(0,44,16,18,Dino[1]);
	}
	else
	{
		OLED_ShowImage(0,44-Jump_Pos,16,18,Dino[2]);
	}
	/* Shrink dino hitbox slightly to reduce false collision feeling. */
	dino.minX=2;
	dino.maxX=14;
	dino.minY=44-Jump_Pos+2;
	dino.maxY=62-Jump_Pos-1;
}
int Collision(struct Object_Position *a,struct Object_Position *b)
{
	if((a->maxX>b->minX)&&(a->minX<b->maxX)&&(a->maxY>b->minY)&&(a->minY<b->maxY))
	{
		OLED_Clear();
		OLED_ShowString(28,24,"Game Over",OLED_8X16);
		OLED_Update();
		Delay_s(1);
		OLED_Clear();
		OLED_Update();
		return 1;
	}
	return 0;
}
int DinoGame_Animation(void)
{
	while(1)
	{
		int return_flag;
		/* Advance game timing counters before rendering each frame. */
		Dino_Tick();
		OLED_Clear();
		Show_Score();
		Show_Ground();
		Show_Barrier();
		Show_Cloud();
		Show_Dino();
		OLED_Update();
		return_flag=Collision(&dino,&Barrier);
		if(return_flag==1)
		{
			return 0;
		}
		Delay_ms(5);
	}
	
//	return 1;
}
void Dino_Tick(void)
{
	uint32_t now_ms;
	uint32_t dt_ms;

	now_ms = HAL_GetTick();
	dt_ms = now_ms - g_dino_last_tick_ms;
	if (dt_ms == 0U)
	{
		return;
	}
	g_dino_last_tick_ms = now_ms;

	/* Score: +1 per 120ms. */
	g_score_acc_ms += dt_ms;
	while (g_score_acc_ms >= DINO_SCORE_STEP_MS)
	{
		g_score_acc_ms -= DINO_SCORE_STEP_MS;
		Score++;
	}

	/* Ground and barrier: move 1px per 14ms for balanced gameplay. */
	g_ground_acc_ms += dt_ms;
	while (g_ground_acc_ms >= DINO_GROUND_STEP_MS)
	{
		g_ground_acc_ms -= DINO_GROUND_STEP_MS;
		Ground_Pos++;
		barrier_pos++;
		if (Ground_Pos >= 256U)
		{
			Ground_Pos = 0U;
		}
		if (barrier_pos >= 144U)
		{
			barrier_pos = 0U;
			barrier_flag = Dino_NextBarrierType(barrier_flag);
		}
	}

	/* Cloud: slower than ground. */
	g_cloud_acc_ms += dt_ms;
	while (g_cloud_acc_ms >= DINO_CLOUD_STEP_MS)
	{
		g_cloud_acc_ms -= DINO_CLOUD_STEP_MS;
		Cloud_Pos++;
		if (Cloud_Pos >= 188U)
		{
			Cloud_Pos = 0U;
		}
	}

	/* Jump animation time base. */
	if (jump_flag == 1U)
	{
		jump_t += dt_ms;
		if (jump_t >= DINO_JUMP_TIME_MS)
		{
			jump_t = 0U;
			jump_flag = 0U;
		}
	}
	else
	{
		g_jump_acc_ms = 0U;
	}
}
void DinoGame_Pos_Init(void)
{
	srand(HAL_GetTick() ^ DWT->CYCCNT);
	Score = 0;
	Ground_Pos = 0;
	Cloud_Pos = 0;
	Jump_Pos = 0;
	jump_t = 0;
	jump_flag = 0;
	barrier_flag = Dino_NextBarrierType(3);
	barrier_pos = rand() % 40;
	g_dino_last_tick_ms = HAL_GetTick();
	g_score_acc_ms = 0U;
	g_ground_acc_ms = 0U;
	g_cloud_acc_ms = 0U;
	g_jump_acc_ms = 0U;
}
