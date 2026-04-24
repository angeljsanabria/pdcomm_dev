/**
 ******************************************************************************
 * @file    bsp_button.h
 * @brief   Board Support Package - Button driver
 * @details This file provides functions to read the board button state.
 ******************************************************************************
 */

#ifndef BSP_BUTTON_H
#define BSP_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Button Pin Definitions (extracted from generated project) */
#define BSP_BUTTON_PIN          GPIO_PIN_6
#define BSP_BUTTON_PORT         GPIOC

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void BSP_Button_Init(void);
uint8_t BSP_Button_Read(void);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* BSP_BUTTON_H */
