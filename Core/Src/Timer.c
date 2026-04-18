#include "main.h"                  // Device header

extern TIM_HandleTypeDef htim2;

void Timer_Init(void)
{
    uint32_t tim_clk;
    uint32_t pclk1;

    __HAL_RCC_TIM2_CLK_ENABLE();

    pclk1 = HAL_RCC_GetPCLK1Freq();
    if ((RCC->CFGR & RCC_CFGR_PPRE1) == RCC_HCLK_DIV1)
    {
        tim_clk = pclk1;
    }
    else
    {
        tim_clk = pclk1 * 2U;
    }

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = (uint16_t)(tim_clk / 10000U - 1U);
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 10 - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(TIM2_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
}

