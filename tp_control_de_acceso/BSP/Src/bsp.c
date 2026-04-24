/**
 ******************************************************************************
 * @file    bsp.c
 * @brief   Board Support Package - Main implementation
 * @details This file provides the main BSP initialization function that
 *          initializes all board peripherals (LEDs, buttons, etc.)
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "bsp_leds.h"
#include "bsp_button.h"
//#include "bsp_uart3.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Inicio de perifericos
 * @retval None
 * @note   This function initializes:
 *         - LEDs
 *         - Buttons
 *         - UART DATA
 *         - I2C-1
 */
void BSP_Init(void)
{
    /* Initialize LEDs */
    BSP_LED_Init();

    /* Initialize Button */
    BSP_Button_Init();

    /* I2C y UART se incian desde fuera. */

    return;
}
