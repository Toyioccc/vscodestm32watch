#include "settime.h"
#include "MyRTC.h"
#include "OLED.h"
#include "key.h"
#include "cmsis_os.h"

extern const uint8_t Diode[];
extern uint8_t KeyNum;

/* GB2312 bytes */
#define CN_YEAR   "\xC4\xEA"
#define CN_MONTH  "\xD4\xC2"
#define CN_DAY    "\xC8\xD5"
#define CN_HOUR   "\xCA\xB1"
#define CN_MIN    "\xB7\xD6"
#define CN_SEC    "\xC3\xEB"
#define CN_RETURN "\xB7\xB5\xBB\xD8"

static void Show_FirstUI(void)
{
	OLED_ShowImage(0, 0, 16, 16, Return);
	OLED_Printf(20, 0, OLED_8X16, "%s", CN_RETURN);
	OLED_Printf(0, 16, OLED_8X16, "%s:%4d", CN_YEAR, MyRTC_Time[0]);
	OLED_Printf(0, 32, OLED_8X16, "%s:%2d", CN_MONTH, MyRTC_Time[1]);
	OLED_Printf(0, 48, OLED_8X16, "%s:%2d", CN_DAY, MyRTC_Time[2]);
}

static void Show_SecondUI(void)
{
	OLED_Printf(0, 0, OLED_8X16, "%s:%2d", CN_HOUR, MyRTC_Time[3]);
	OLED_Printf(0, 16, OLED_8X16, "%s:%2d", CN_MIN, MyRTC_Time[4]);
	OLED_Printf(0, 32, OLED_8X16, "%s:%2d", CN_SEC, MyRTC_Time[5]);
}

static void Change_RTC_Time(uint8_t i, uint8_t increase)
{
	if (increase)
		MyRTC_Time[i]++;
	else
		MyRTC_Time[i]--;
	MyRTC_SetTime();
}

/* ==================== Page: select which field to modify ==================== */

UiPage_t Page_SelField(void)
{
	static int sel = 1;

	if (KeyNum == 1)
	{
		sel--;
		if (sel <= 0) sel = 7;
	}
	else if (KeyNum == 2)
	{
		sel++;
		if (sel >= 8) sel = 1;
	}
	else if (KeyNum == 3)
	{
		OLED_Clear();
		OLED_Update();
		switch (sel)
		{
			case 1: return UI_PAGE_SETTING;
			case 2: return UI_PAGE_ADJ_YEAR;
			case 3: return UI_PAGE_ADJ_MONTH;
			case 4: return UI_PAGE_ADJ_DAY;
			case 5: return UI_PAGE_ADJ_HOUR;
			case 6: return UI_PAGE_ADJ_MIN;
			case 7: return UI_PAGE_ADJ_SEC;
		}
	}

	OLED_Clear();
	switch (sel)
	{
		case 1: Show_FirstUI();  OLED_ReverseArea(0, 0, 16, 16);  break;
		case 2: Show_FirstUI();  OLED_ReverseArea(0, 16, 16, 16); break;
		case 3: Show_FirstUI();  OLED_ReverseArea(0, 32, 16, 16); break;
		case 4: Show_FirstUI();  OLED_ReverseArea(0, 48, 16, 16); break;
		case 5: Show_SecondUI(); OLED_ReverseArea(0, 0, 16, 16);  break;
		case 6: Show_SecondUI(); OLED_ReverseArea(0, 16, 16, 16); break;
		case 7: Show_SecondUI(); OLED_ReverseArea(0, 32, 16, 16); break;
	}
	OLED_Update();
	return UI_PAGE_SEL_FIELD;
}

/* ==================== Value adjustment pages ==================== */

UiPage_t Page_AdjYear(void)
{
	if (KeyNum == 1)
	{
		Change_RTC_Time(0, 1);
	}
	else if (KeyNum == 2)
	{
		Change_RTC_Time(0, 0);
	}
	else if (KeyNum == 3)
	{
		return UI_PAGE_SEL_FIELD;
	}

	Show_FirstUI();
	OLED_ReverseArea(24, 16, 32, 16);
	OLED_Update();
	return UI_PAGE_ADJ_YEAR;
}

UiPage_t Page_AdjMonth(void)
{
	if (KeyNum == 1)
	{
		Change_RTC_Time(1, 1);
		if (MyRTC_Time[1] >= 13) { MyRTC_Time[1] = 1; MyRTC_SetTime(); }
	}
	else if (KeyNum == 2)
	{
		Change_RTC_Time(1, 0);
		if (MyRTC_Time[1] >= 13) { MyRTC_Time[1] = 12; MyRTC_SetTime(); }
	}
	else if (KeyNum == 3)
	{
		return UI_PAGE_SEL_FIELD;
	}

	Show_FirstUI();
	OLED_ReverseArea(24, 32, 16, 16);
	OLED_Update();
	return UI_PAGE_ADJ_MONTH;
}

UiPage_t Page_AdjDay(void)
{
	if (KeyNum == 1)
	{
		Change_RTC_Time(2, 1);
		if (MyRTC_Time[2] >= 32) { MyRTC_Time[2] = 1; MyRTC_SetTime(); }
	}
	else if (KeyNum == 2)
	{
		Change_RTC_Time(2, 0);
		if (MyRTC_Time[2] >= 32) { MyRTC_Time[2] = 31; MyRTC_SetTime(); }
	}
	else if (KeyNum == 3)
	{
		return UI_PAGE_SEL_FIELD;
	}

	Show_FirstUI();
	OLED_ReverseArea(24, 48, 16, 16);
	OLED_Update();
	return UI_PAGE_ADJ_DAY;
}

UiPage_t Page_AdjHour(void)
{
	if (KeyNum == 1)
	{
		Change_RTC_Time(3, 1);
		if (MyRTC_Time[3] >= 24) { MyRTC_Time[3] = 0; MyRTC_SetTime(); }
	}
	else if (KeyNum == 2)
	{
		Change_RTC_Time(3, 0);
		if (MyRTC_Time[3] >= 24) { MyRTC_Time[3] = 23; MyRTC_SetTime(); }
	}
	else if (KeyNum == 3)
	{
		return UI_PAGE_SEL_FIELD;
	}

	Show_SecondUI();
	OLED_ReverseArea(24, 0, 16, 16);
	OLED_Update();
	return UI_PAGE_ADJ_HOUR;
}

UiPage_t Page_AdjMin(void)
{
	if (KeyNum == 1)
	{
		Change_RTC_Time(4, 1);
		if (MyRTC_Time[4] >= 60) { MyRTC_Time[4] = 0; MyRTC_SetTime(); }
	}
	else if (KeyNum == 2)
	{
		Change_RTC_Time(4, 0);
		if (MyRTC_Time[4] >= 60) { MyRTC_Time[4] = 59; MyRTC_SetTime(); }
	}
	else if (KeyNum == 3)
	{
		return UI_PAGE_SEL_FIELD;
	}

	Show_SecondUI();
	OLED_ReverseArea(24, 16, 16, 16);
	OLED_Update();
	return UI_PAGE_ADJ_MIN;
}

UiPage_t Page_AdjSec(void)
{
	if (KeyNum == 1)
	{
		Change_RTC_Time(5, 1);
		if (MyRTC_Time[5] >= 60) { MyRTC_Time[5] = 0; MyRTC_SetTime(); }
	}
	else if (KeyNum == 2)
	{
		Change_RTC_Time(5, 0);
		if (MyRTC_Time[5] >= 60) { MyRTC_Time[5] = 59; MyRTC_SetTime(); }
	}
	else if (KeyNum == 3)
	{
		return UI_PAGE_SEL_FIELD;
	}

	Show_SecondUI();
	OLED_ReverseArea(24, 32, 16, 16);
	OLED_Update();
	return UI_PAGE_ADJ_SEC;
}
