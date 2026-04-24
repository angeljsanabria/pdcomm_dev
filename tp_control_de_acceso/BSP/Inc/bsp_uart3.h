/**
 ******************************************************************************
 * @file    bsp_uart3.h
 * @brief   BSP - USART3 (ST-LINK VCP on NUCLEO-L4R5ZI)
 ******************************************************************************
 */

#ifndef BSP_UART3_H
#define BSP_UART3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"

/* Pin mux: PD8 TX, PD9 RX (same net names as ST board / Cube) */
#define BSP_USART3_TX_PIN        GPIO_PIN_8
#define BSP_USART3_RX_PIN        GPIO_PIN_9
#define BSP_USART3_GPIO_PORT     GPIOD
#define BSP_USART3_GPIO_AF       GPIO_AF7_USART3



void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);


#ifdef __cplusplus
}
#endif

#endif /* BSP_UART3_H */
