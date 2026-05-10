/* USER CODE BEGIN Header */
/**
  *******************************************************************************
  * @file           : menu.h
  * @brief          : Header for menu.c file.
  *                   This file contains the function declarations for menu operations.
  *******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_H
#define __MENU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  UI_PAGE_CLOCK = 0,
  UI_PAGE_MENU,
  UI_PAGE_SETTING,
  UI_PAGE_SEL_FIELD,
  UI_PAGE_ADJ_YEAR,
  UI_PAGE_ADJ_MONTH,
  UI_PAGE_ADJ_DAY,
  UI_PAGE_ADJ_HOUR,
  UI_PAGE_ADJ_MIN,
  UI_PAGE_ADJ_SEC,
  UI_PAGE_GAME,
  UI_PAGE_EMOJI,
} UiPage_t;
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Peripheral_Init(void);
int Menu(void);
int SettingPage(void);
int First_Page_Clock(void);
void StopWatch_Tick(void);
int StopWatch(void);
int LED(void);
int MPU6050(void);
int Game(void);
int Emoji(void);
int Gradienter(void);
void Battery_ReadADC(void);
UiPage_t Page_SelField(void);
UiPage_t Page_AdjYear(void);
UiPage_t Page_AdjMonth(void);
UiPage_t Page_AdjDay(void);
UiPage_t Page_AdjHour(void);
UiPage_t Page_AdjMin(void);
UiPage_t Page_AdjSec(void);
UiPage_t Page_Game(void);
UiPage_t Page_Emoji(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __MENU_H */