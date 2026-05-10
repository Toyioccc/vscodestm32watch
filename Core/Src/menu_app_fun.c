#include "main.h"
#include "menu.h"
#include "OLED.h"
#include "key.h"
#include "led.h"
#include "delay.h"
#include "dino.h"

extern uint8_t KeyNum;

/* GB2312 bytes: "谷歌小恐龙" */
#define CN_DINO_TEXT "\xB9\xC8\xB8\xE8\xD0\xA1\xBF\xD6\xC1\xFA"

static void Show_Game_UI(void)
{
	OLED_ShowImage(0, 0, 16, 16, Return);
	OLED_ShowString(0, 16, CN_DINO_TEXT, OLED_8X16);
}

/* ==================== Page: Game ==================== */

UiPage_t Page_Game(void)
{
	static int sel = 1;

	if (KeyNum == 1)
	{
		sel--;
		if (sel <= 0) sel = 2;
	}
	else if (KeyNum == 2)
	{
		sel++;
		if (sel >= 3) sel = 1;
	}
	else if (KeyNum == 3)
	{
		OLED_Clear();
		OLED_Update();
		if (sel == 1)
			return UI_PAGE_MENU;
		if (sel == 2)
		{
			DinoGame_Pos_Init();
			DinoGame_Animation();
			return UI_PAGE_MENU;
		}
	}

	Show_Game_UI();
	if (sel == 1)
		OLED_ReverseArea(0, 0, 16, 16);
	else
	{
		LED1_OFF();
		OLED_ReverseArea(0, 16, 80, 16);
	}
	OLED_Update();
	return UI_PAGE_GAME;
}

/* ==================== Page: Emoji ==================== */

UiPage_t Page_Emoji(void)
{
	static uint32_t anim_start = 0;

	if (KeyNum == 3)
	{
		OLED_Clear();
		OLED_Update();
		anim_start = 0;
		return UI_PAGE_MENU;
	}

	uint32_t now = HAL_GetTick();
	if (anim_start == 0)
		anim_start = now;

	uint32_t cycle = (now - anim_start) % 1100;  /* 3*100 + 3*100 + 500 = 1100 ms */
	uint8_t  fi;
	uint8_t  opening;

	if (cycle < 300)
	{
		fi = (uint8_t)(cycle / 100);       /* 0, 1, 2 */
		opening = 1;
	}
	else if (cycle < 600)
	{
		fi = (uint8_t)((cycle - 300) / 100); /* 0, 1, 2 */
		opening = 0;
	}
	else
	{
		fi = 2;
		opening = 0;                        /* pause frame */
	}

	OLED_Clear();
	if (opening)
	{
		OLED_ShowImage(30, 10 + fi, 16, 16, eyebrow[0]);
		OLED_ShowImage(82, 10 + fi, 16, 16, eyebrow[1]);
		OLED_DrawEllipse(40, 32, 6, 6 - fi, 1);
		OLED_DrawEllipse(88, 32, 6, 6 - fi, 1);
	}
	else
	{
		OLED_ShowImage(30, 12 - fi, 16, 16, eyebrow[0]);
		OLED_ShowImage(82, 12 - fi, 16, 16, eyebrow[1]);
		OLED_DrawEllipse(40, 32, 6, 4 + fi, 1);
		OLED_DrawEllipse(88, 32, 6, 4 + fi, 1);
	}
	OLED_ShowImage(54, 40, 20, 20, mouth);
	OLED_Update();
	return UI_PAGE_EMOJI;
}
