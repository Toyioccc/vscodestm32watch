/**
  ******************************************************************************
  * @file           : delay.c
  * @brief          : Delay functions
  ******************************************************************************
  */

#include "delay.h"

static void DWT_Delay_Init(void)
{
    if ((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) == 0)
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void Delay_us(uint32_t xus)
{
    uint32_t cycles = (SystemCoreClock / 1000000U) * xus;

    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0)
    {
        DWT_Delay_Init();
    }

    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < cycles)
    {
        __NOP();
    }
}

void Delay_ms(uint32_t xms)
{
    HAL_Delay(xms);
}

void Delay_s(uint32_t xs)
{
    while (xs--)
    {
        HAL_Delay(1000);
    }
}
