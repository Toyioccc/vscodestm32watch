#include "AD.h"
#include "stm32f1xx_hal.h"

void AD_Init(void)
{
	/* Enable GPIOA and ADC1 clocks, then sync */
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;
	__DSB();

	/* ADC clock prescaler: PCLK2 / 6 (clear-then-set) */
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_ADCPRE) | RCC_CFGR_ADCPRE_DIV6;

	/* PA0: analog input */
	GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);

	/* ADC1: single conversion, software trigger, right alignment */
	ADC1->CR1 = 0;
	ADC1->CR2 = ADC_CR2_EXTSEL | ADC_CR2_ADON;
	ADC1->SMPR2 = ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_0;
	ADC1->SQR1 = 0;
	ADC1->SQR3 = 0;

	/* Stabilization delay (t_STAB ≈ 1µs, generous margin) */
	for (volatile uint32_t i = 0; i < 100000; i++);

	/* Calibrate with timeout */
	ADC1->CR2 |= ADC_CR2_CAL;
	{
		volatile uint32_t cal_timeout = 1000000;
		while ((ADC1->CR2 & ADC_CR2_CAL) && --cal_timeout);
	}

	/* Dummy conversion: first reading after power-up is unreliable */
	ADC1->CR2 |= ADC_CR2_SWSTART;
	for (volatile uint32_t i = 0; i < 100000; i++);
	(void)ADC1->DR;
}

uint16_t AD_GetValue(void)
{
	ADC1->CR2 |= ADC_CR2_ADON;
	(void)ADC1->DR;                  /* clear stale EOC */
	ADC1->CR2 |= ADC_CR2_SWSTART;   /* start conversion */
	while (!(ADC1->SR & ADC_SR_EOC));
	return (uint16_t)(ADC1->DR & 0xFFF);
}
