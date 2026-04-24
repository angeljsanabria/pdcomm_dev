	/**
 ******************************************************************************
 * @file    bsp_button.c
 * @brief   Board Support Package - Button driver implementation
 * @details This file implements button read functions.
 *          GPIO configuration extracted from generated project.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_button.h"
#include "stm32l4xx_hal.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static GPIO_InitTypeDef GPIO_InitStruct = {0};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initialize button
 * @retval None
 * @note   Extracted from MX_GPIO_Init() - Button configuration
 */
void BSP_Button_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure GPIO pin : B1_Pin */
    GPIO_InitStruct.Pin = BSP_BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BSP_BUTTON_PORT, &GPIO_InitStruct);
}

/**
 * @brief  Read button state
 * @retval 1 if button is pressed, 0 if released
 * @note   Button is active high (pressed = HIGH)
 */
uint8_t BSP_Button_Read(void)
{
    return (uint8_t)HAL_GPIO_ReadPin(BSP_BUTTON_PORT, BSP_BUTTON_PIN);
}
