#include "main.h"
#include <time.h>

uint16_t MyRTC_Time[] = {2026, 3, 10, 23, 7, 30};
static uint32_t g_last_rtc_ms;

static uint8_t Is_Leap_Year(uint16_t year)
{
    return ((year % 4U == 0U) && (year % 100U != 0U)) || (year % 400U == 0U);
}

static uint8_t Days_In_Month(uint16_t year, uint8_t month)
{
    static const uint8_t days[12] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    if (month == 2U)
    {
        return (uint8_t)(days[1] + (Is_Leap_Year(year) ? 1U : 0U));
    }

    if (month >= 1U && month <= 12U)
    {
        return days[month - 1U];
    }

    return 31U;
}

static void RTC_Increment_One_Second(void)
{
    MyRTC_Time[5]++;
    if (MyRTC_Time[5] < 60U)
    {
        return;
    }

    MyRTC_Time[5] = 0U;
    MyRTC_Time[4]++;
    if (MyRTC_Time[4] < 60U)
    {
        return;
    }

    MyRTC_Time[4] = 0U;
    MyRTC_Time[3]++;
    if (MyRTC_Time[3] < 24U)
    {
        return;
    }

    MyRTC_Time[3] = 0U;
    MyRTC_Time[2]++;

    if (MyRTC_Time[2] <= Days_In_Month(MyRTC_Time[0], (uint8_t)MyRTC_Time[1]))
    {
        return;
    }

    MyRTC_Time[2] = 1U;
    MyRTC_Time[1]++;
    if (MyRTC_Time[1] <= 12U)
    {
        return;
    }

    MyRTC_Time[1] = 1U;
    MyRTC_Time[0]++;
}

void MyRTC_Init(void)
{
    /* Software RTC base time. */
    g_last_rtc_ms = HAL_GetTick();
}

void MyRTC_ReadTime(void)
{
    uint32_t now_ms = HAL_GetTick();

    while ((now_ms - g_last_rtc_ms) >= 1000U)
    {
        g_last_rtc_ms += 1000U;
        RTC_Increment_One_Second();
    }
}

void MyRTC_SetTime(void)
{
    /* Reset software RTC base when upper layer updates time. */
    g_last_rtc_ms = HAL_GetTick();
}
