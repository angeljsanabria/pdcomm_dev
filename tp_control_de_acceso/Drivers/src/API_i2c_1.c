/*
 * API_i2c_1.c
 *
 *  Created on: 16 abr 2026
 *      Author: angel-dev
 */

#include "API_i2c_1.h"
#include "main.h"

// Dejo la variable privada, y hago un getter

static I2C_HandleTypeDef hi2c1;


void I2C1_Init(void)		// Pasar a la API
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  //hi2c1.Init.Timing = 0x00000E14;
  hi2c1.Init.Timing = 0x00000F3B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
  return;
}


// Getter que deevuelve el puntero del handler
I2C_HandleTypeDef* I2C1_Get_Handler(void) {
    return &hi2c1;
}

HAL_StatusTypeDef I2C1_Write(uint16_t addr, uint8_t* pData, uint16_t size, uint32_t timeout) {
    return HAL_I2C_Master_Transmit(&hi2c1, addr, pData, size, timeout);
}

HAL_StatusTypeDef I2C1_Read(uint16_t addr, uint8_t* pData, uint16_t size, uint32_t timeout) {
    return HAL_I2C_Master_Receive(&hi2c1, addr, pData, size, timeout);
}

