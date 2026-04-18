#include "settime.h"

#include "MyRTC.h"
#include "OLED.h"
#include "key.h"

extern const uint8_t Diode[];

/* GB2312 bytes */
#define CN_YEAR   "\xC4\xEA"
#define CN_MONTH  "\xD4\xC2"
#define CN_DAY    "\xC8\xD5"
#define CN_HOUR   "\xCA\xB1"
#define CN_MIN    "\xB7\xD6"
#define CN_SEC    "\xC3\xEB"
#define CN_RETURN "\xB7\xB5\xBB\xD8"

static void Show_SetTime_FirstUI(void)
{
    OLED_ShowImage(0, 0, 16, 16, Return);
    OLED_Printf(20, 0, OLED_8X16, "%s", CN_RETURN);
    OLED_Printf(0, 16, OLED_8X16, "%s:%4d", CN_YEAR, MyRTC_Time[0]);
    OLED_Printf(0, 32, OLED_8X16, "%s:%2d", CN_MONTH, MyRTC_Time[1]);
    OLED_Printf(0, 48, OLED_8X16, "%s:%2d", CN_DAY, MyRTC_Time[2]);
}

static void Show_SetTime_SecondUI(void)
{
    OLED_Printf(0, 0, OLED_8X16, "%s:%2d", CN_HOUR, MyRTC_Time[3]);
    OLED_Printf(0, 16, OLED_8X16, "%s:%2d", CN_MIN, MyRTC_Time[4]);
    OLED_Printf(0, 32, OLED_8X16, "%s:%2d", CN_SEC, MyRTC_Time[5]);
}

static void Change_RTC_Time(uint8_t i, uint8_t increase)
{
    if (increase)
    {
        MyRTC_Time[i]++;
    }
    else
    {
        MyRTC_Time[i]--;
    }
    MyRTC_SetTime();
}

static int SetYear(void)
{
    while (1)
    {
        uint8_t keyNum = Key_GetNum();
        if (keyNum == 1)
        {
            Change_RTC_Time(0, 1);
        }
        else if (keyNum == 2)
        {
            Change_RTC_Time(0, 0);
        }
        else if (keyNum == 3)
        {
            return 0;
        }

        Show_SetTime_FirstUI();
        OLED_ReverseArea(24, 16, 32, 16);
        OLED_Update();
    }
}

static int SetMonth(void)
{
    while (1)
    {
        uint8_t keyNum = Key_GetNum();
        if (keyNum == 1)
        {
            Change_RTC_Time(1, 1);
            if (MyRTC_Time[1] >= 13)
            {
                MyRTC_Time[1] = 1;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 2)
        {
            Change_RTC_Time(1, 0);
            if (MyRTC_Time[1] >= 13)
            {
                MyRTC_Time[1] = 12;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 3)
        {
            return 0;
        }

        Show_SetTime_FirstUI();
        OLED_ReverseArea(24, 32, 16, 16);
        OLED_Update();
    }
}

static int SetDay(void)
{
    while (1)
    {
        uint8_t keyNum = Key_GetNum();
        if (keyNum == 1)
        {
            Change_RTC_Time(2, 1);
            if (MyRTC_Time[2] >= 32)
            {
                MyRTC_Time[2] = 1;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 2)
        {
            Change_RTC_Time(2, 0);
            if (MyRTC_Time[2] >= 32)
            {
                MyRTC_Time[2] = 31;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 3)
        {
            return 0;
        }

        Show_SetTime_FirstUI();
        OLED_ReverseArea(24, 48, 16, 16);
        OLED_Update();
    }
}

static int SetHour(void)
{
    while (1)
    {
        uint8_t keyNum = Key_GetNum();
        if (keyNum == 1)
        {
            Change_RTC_Time(3, 1);
            if (MyRTC_Time[3] >= 24)
            {
                MyRTC_Time[3] = 0;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 2)
        {
            Change_RTC_Time(3, 0);
            if (MyRTC_Time[3] >= 24)
            {
                MyRTC_Time[3] = 23;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 3)
        {
            return 0;
        }

        Show_SetTime_SecondUI();
        OLED_ReverseArea(24, 0, 16, 16);
        OLED_Update();
    }
}

static int SetMin(void)
{
    while (1)
    {
        uint8_t keyNum = Key_GetNum();
        if (keyNum == 1)
        {
            Change_RTC_Time(4, 1);
            if (MyRTC_Time[4] >= 60)
            {
                MyRTC_Time[4] = 0;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 2)
        {
            Change_RTC_Time(4, 0);
            if (MyRTC_Time[4] >= 60)
            {
                MyRTC_Time[4] = 59;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 3)
        {
            return 0;
        }

        Show_SetTime_SecondUI();
        OLED_ReverseArea(24, 16, 16, 16);
        OLED_Update();
    }
}

static int SetSec(void)
{
    while (1)
    {
        uint8_t keyNum = Key_GetNum();
        if (keyNum == 1)
        {
            Change_RTC_Time(5, 1);
            if (MyRTC_Time[5] >= 60)
            {
                MyRTC_Time[5] = 0;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 2)
        {
            Change_RTC_Time(5, 0);
            if (MyRTC_Time[5] >= 60)
            {
                MyRTC_Time[5] = 59;
                MyRTC_SetTime();
            }
        }
        else if (keyNum == 3)
        {
            return 0;
        }

        Show_SetTime_SecondUI();
        OLED_ReverseArea(24, 32, 16, 16);
        OLED_Update();
    }
}

int SetTime(void)
{
    int set_time_flag = 1;

    while (1)
    {
        uint8_t keyNum = Key_GetNum();
        uint8_t set_time_flag_temp = 0;

        if (keyNum == 1)
        {
            set_time_flag--;
            if (set_time_flag <= 0)
            {
                set_time_flag = 7;
            }
        }
        else if (keyNum == 2)
        {
            set_time_flag++;
            if (set_time_flag >= 8)
            {
                set_time_flag = 1;
            }
        }
        else if (keyNum == 3)
        {
            OLED_Clear();
            OLED_Update();
            set_time_flag_temp = (uint8_t)set_time_flag;
        }

        if (set_time_flag_temp == 1)
        {
            return 0;
        }
        else if (set_time_flag_temp == 2)
        {
            SetYear();
        }
        else if (set_time_flag_temp == 3)
        {
            SetMonth();
        }
        else if (set_time_flag_temp == 4)
        {
            SetDay();
        }
        else if (set_time_flag_temp == 5)
        {
            SetHour();
        }
        else if (set_time_flag_temp == 6)
        {
            SetMin();
        }
        else if (set_time_flag_temp == 7)
        {
            SetSec();
        }

        switch (set_time_flag)
        {
            case 1:
                OLED_Clear();
                Show_SetTime_FirstUI();
                OLED_ReverseArea(0, 0, 16, 16);
                OLED_Update();
                break;

            case 2:
                OLED_Clear();
                Show_SetTime_FirstUI();
                OLED_ReverseArea(0, 16, 16, 16);
                OLED_Update();
                break;

            case 3:
                OLED_Clear();
                Show_SetTime_FirstUI();
                OLED_ReverseArea(0, 32, 16, 16);
                OLED_Update();
                break;

            case 4:
                OLED_Clear();
                Show_SetTime_FirstUI();
                OLED_ReverseArea(0, 48, 16, 16);
                OLED_Update();
                break;

            case 5:
                OLED_Clear();
                Show_SetTime_SecondUI();
                OLED_ReverseArea(0, 0, 16, 16);
                OLED_Update();
                break;

            case 6:
                OLED_Clear();
                Show_SetTime_SecondUI();
                OLED_ReverseArea(0, 16, 16, 16);
                OLED_Update();
                break;

            case 7:
                OLED_Clear();
                Show_SetTime_SecondUI();
                OLED_ReverseArea(0, 32, 16, 16);
                OLED_Update();
                break;

            default:
                break;
        }
    }
}
