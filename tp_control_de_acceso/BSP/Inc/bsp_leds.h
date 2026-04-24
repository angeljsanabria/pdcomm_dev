/**
 ******************************************************************************
 * @file    bsp_leds.h
 * @brief   Board Support Package - LED driver
 * @details This file provides functions to control the board LEDs.
 ******************************************************************************
 */

#ifndef BSP_LED_H
#define BSP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* LED Definitions */
#define BSP_LED2                0
#define BSP_LED3                1
#define BSP_LED4                2
#define BSP_LED5                3
#define BSP_LED6                4
#define BSP_LED_ROJO			BSP_LED4
#define BSP_LED_AMARILLO		BSP_LED5
#define BSP_LED_VERDE			BSP_LED6

/* LED Pin Definitions (extracted from generated project) */
#define BSP_LED2_PIN            GPIO_PIN_7
#define BSP_LED2_PORT           GPIOB
#define BSP_LED3_PIN            GPIO_PIN_14
#define BSP_LED3_PORT           GPIOB
#define BSP_LED4_PIN            GPIO_PIN_10
#define BSP_LED4_PORT           GPIOB
#define BSP_LED5_PIN            GPIO_PIN_11
#define BSP_LED5_PORT           GPIOB
#define BSP_LED6_PIN            GPIO_PIN_0
#define BSP_LED6_PORT           GPIOB

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void BSP_LED_Init(void);
void BSP_LED_On(uint8_t led);
void BSP_LED_Off(uint8_t led);
void BSP_LED_Toggle(uint8_t led);
uint8_t BSP_LED_IsOn(uint8_t led);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* BSP_LED_H */
