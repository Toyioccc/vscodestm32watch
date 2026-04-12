#include"main.h"

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
	Barrier.minX=127-barrier_pos;
	Barrier.maxX=143-barrier_pos;
	Barrier.minY=44;
	Barrier.maxY=62;
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
	KeyNum=Key_GetNum();
	if(KeyNum==1)
		jump_flag=1;
	Jump_Pos=28*sin((float)(pi*jump_t/1000));
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
	dino.minX=0;
	dino.maxX=16;
	dino.minY=44-Jump_Pos;
	dino.maxY=62-Jump_Pos;
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
	}
	
//	return 1;
}
void Dino_Tick(void)//设置分频，实现每0.1秒自增
{
	static uint8_t Score_Count,Ground_Count,Cloud_Count;
	Score_Count++;
	Ground_Count++;
	Cloud_Count++;
	if(Score_Count>=100)
	{
		Score_Count=0;
		Score++;
	}
	if(Ground_Count>=20)
	{
		Ground_Count=0;
		Ground_Pos++;
		barrier_pos++;
		if(Ground_Pos>=256)
			Ground_Pos=0;
		if(barrier_pos>=144)
		{
			barrier_pos=0;
			barrier_flag = Dino_NextBarrierType(barrier_flag);
		}
	}
	if(Cloud_Count>=50)
	{
		Cloud_Count=0;
		Cloud_Pos++;
		if(Cloud_Pos>=188)
			Cloud_Pos=0;
	}
	if(jump_flag==1)
	{
		jump_t++;
		if(jump_t>=1000)
		{
			jump_t=0;
			jump_flag=0;
		}
	}
}
void DinoGame_Pos_Init(void)
{
	srand(HAL_GetTick() ^ DWT->CYCCNT);
	Score = 0;
	Ground_Pos = 0;
	Cloud_Pos = 0;
	Jump_Pos = 0;
	barrier_flag = Dino_NextBarrierType(3);
	barrier_pos = rand() % 40;
}
