/**
 ******************************************************************************
 * @file    bsp_uart3.c
 * @brief   BSP - USART3: HAL_UART_MspInit/DeInit, HAL_UART_Init
 * @note    HAL_UART_Init() llama a HAL_UART_MspInit(); esta definida aqui para
 *          que todo el UART3 quede en BSP (stm32l4xx_hal_msp.c sin UART).
 ******************************************************************************
 */

#include "bsp_uart3.h"
#include "main.h"


static void usart3_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  if (huart->Instance != USART3)
  {
    return;
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_RCC_USART3_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  GPIO_InitStruct.Pin = BSP_USART3_TX_PIN | BSP_USART3_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = BSP_USART3_GPIO_AF;
  HAL_GPIO_Init(BSP_USART3_GPIO_PORT, &GPIO_InitStruct);
}

static void usart3_MspDeInit(UART_HandleTypeDef *huart)
{
  if (huart->Instance != USART3)
  {
    return;
  }

  __HAL_RCC_USART3_CLK_DISABLE();
  HAL_GPIO_DeInit(BSP_USART3_GPIO_PORT, BSP_USART3_TX_PIN | BSP_USART3_RX_PIN);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  usart3_MspInit(huart);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  usart3_MspDeInit(huart);
}

