/**
 ******************************************************************************
 * @file    bsp_led.c
 * @brief   Board Support Package - LED driver implementation
 * @details This file implements LED control functions.
 *          GPIO configuration extracted from generated project.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_leds.h"
#include "stm32l4xx_hal.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static GPIO_InitTypeDef GPIO_InitStruct = {0};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initialize LEDs
 * @retval None
 * @note   Extracted from MX_GPIO_Init() - LED configuration
 */
void BSP_LED_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure GPIO pin Output Level - Turn off LEDs initially */
    HAL_GPIO_WritePin(BSP_LED2_PORT, BSP_LED2_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BSP_LED3_PORT, BSP_LED3_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BSP_LED4_PORT, BSP_LED4_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BSP_LED5_PORT, BSP_LED5_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BSP_LED6_PORT, BSP_LED6_PIN, GPIO_PIN_RESET);

    /* Configure GPIO pins : LD3_Pin LD2_Pin */
    GPIO_InitStruct.Pin = BSP_LED3_PIN | BSP_LED2_PIN | BSP_LED4_PIN | BSP_LED5_PIN | BSP_LED6_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
 * @brief  Turn LED on
 * @param  led: LED number (BSP_LED2 or BSP_LED3)
 * @retval None
 */
void BSP_LED_On(uint8_t led)
{
    switch(led) {
        case BSP_LED2:
            HAL_GPIO_WritePin(BSP_LED2_PORT, BSP_LED2_PIN, GPIO_PIN_SET);
            break;
        case BSP_LED3:
            HAL_GPIO_WritePin(BSP_LED3_PORT, BSP_LED3_PIN, GPIO_PIN_SET);
            break;
        case BSP_LED4:
            HAL_GPIO_WritePin(BSP_LED4_PORT, BSP_LED4_PIN, GPIO_PIN_SET);
            break;
        case BSP_LED5:
            HAL_GPIO_WritePin(BSP_LED5_PORT, BSP_LED5_PIN, GPIO_PIN_SET);
            break;
        case BSP_LED6:
        	HAL_GPIO_WritePin(BSP_LED6_PORT, BSP_LED6_PIN, GPIO_PIN_SET);
            break;
        default:
            break;
    }
}

/**
 * @brief  Turn LED off
 * @param  led: LED number (BSP_LED2 or BSP_LED3)
 * @retval None
 */
void BSP_LED_Off(uint8_t led)
{
    switch(led) {
        case BSP_LED2:
            HAL_GPIO_WritePin(BSP_LED2_PORT, BSP_LED2_PIN, GPIO_PIN_RESET);
            break;
        case BSP_LED3:
            HAL_GPIO_WritePin(BSP_LED3_PORT, BSP_LED3_PIN, GPIO_PIN_RESET);
            break;
        case BSP_LED4:
            HAL_GPIO_WritePin(BSP_LED4_PORT, BSP_LED4_PIN, GPIO_PIN_RESET);
        case BSP_LED5:
            HAL_GPIO_WritePin(BSP_LED5_PORT, BSP_LED5_PIN, GPIO_PIN_RESET);
        case BSP_LED6:
            HAL_GPIO_WritePin(BSP_LED6_PORT, BSP_LED6_PIN, GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

/**
 * @brief  Toggle LED
 * @param  led: LED number (BSP_LED2 or BSP_LED3)
 * @retval None
 */
void BSP_LED_Toggle(uint8_t led)
{
    switch(led) {
        case BSP_LED2:
            HAL_GPIO_TogglePin(BSP_LED2_PORT, BSP_LED2_PIN);
            break;
        case BSP_LED3:
            HAL_GPIO_TogglePin(BSP_LED3_PORT, BSP_LED3_PIN);
            break;
        case BSP_LED4:
            HAL_GPIO_TogglePin(BSP_LED4_PORT, BSP_LED4_PIN);
            break;
        case BSP_LED5:
            HAL_GPIO_TogglePin(BSP_LED5_PORT, BSP_LED5_PIN);
            break;
        case BSP_LED6:
            HAL_GPIO_TogglePin(BSP_LED6_PORT, BSP_LED6_PIN);
            break;
        default:
            break;
    }
}

/**
 * @brief  Check del estado de GPIO asociado a un LED
 * @param  LED (BSP_LED2 or BSP_LED3)
 * @retval 1 si el estado del GPIO es Alto (se considera led encendido)
 */
uint8_t BSP_LED_IsOn(uint8_t led)
{
    GPIO_PinState pin;

    switch (led) {
        case BSP_LED2:
            pin = HAL_GPIO_ReadPin(BSP_LED2_PORT, BSP_LED2_PIN);
            break;
        case BSP_LED3:
            pin = HAL_GPIO_ReadPin(BSP_LED3_PORT, BSP_LED3_PIN);
            break;
        // Agregar LED 4, 5, 6 si hace falta
        default:
            return 0;
    }

    if(pin == GPIO_PIN_SET){
        return 1;
    }else{
        return 0;
    }
}
