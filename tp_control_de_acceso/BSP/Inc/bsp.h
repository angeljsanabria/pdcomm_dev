/**
 ******************************************************************************
 * @file    bsp.h
 * @brief   Board Support Package - Main header
 * @details This file provides the main API for board initialization and
 *          hardware abstraction functions.
 ******************************************************************************
 */

#ifndef BSP_H
#define BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "bsp_leds.h"
#include "bsp_button.h"
#include "bsp_uart3.h"
#include "bsp_i2c1.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief  Initialize all board peripherals
 * @retval None
 */
void BSP_Init(void);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* BSP_H */
