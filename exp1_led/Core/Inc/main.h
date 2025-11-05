/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KYE2_Pin GPIO_PIN_0
#define KYE2_GPIO_Port GPIOA
#define KYE2_EXTI_IRQn EXTI0_1_IRQn
#define KEY1_Pin GPIO_PIN_1
#define KEY1_GPIO_Port GPIOA
#define KEY1_EXTI_IRQn EXTI0_1_IRQn
#define LIS2D_CS_Pin GPIO_PIN_4
#define LIS2D_CS_GPIO_Port GPIOA
#define B_LED_Pin GPIO_PIN_6
#define B_LED_GPIO_Port GPIOC
#define R_LED_Pin GPIO_PIN_11
#define R_LED_GPIO_Port GPIOA
#define LIS2D_INT1_Pin GPIO_PIN_12
#define LIS2D_INT1_GPIO_Port GPIOA
#define LIS2D_INT1_EXTI_IRQn EXTI4_15_IRQn
#define IRQ_N_DW1000_Pin GPIO_PIN_15
#define IRQ_N_DW1000_GPIO_Port GPIOA
#define IRQ_N_DW1000_EXTI_IRQn EXTI4_15_IRQn
#define RST_N_DW1000_Pin GPIO_PIN_3
#define RST_N_DW1000_GPIO_Port GPIOB
#define WAKEUP_DW1000_Pin GPIO_PIN_4
#define WAKEUP_DW1000_GPIO_Port GPIOB
#define SPICSN_DW1000_Pin GPIO_PIN_5
#define SPICSN_DW1000_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
