/*
 * API_i2c_1.h
 *
 *  Created on: 16 abr 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_I2C_1_H_
#define API_INC_API_I2C_1_H_

#include "bsp_i2c1.h"

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
void I2C1_Init(void);


/**
  * @brief 	Retorno del puntero handler de I2C1
  *
  * @param 	None
  * @retval I2C_HandleTypeDef puntero de i2c_1
  */
I2C_HandleTypeDef* I2C1_Get_Handler(void);



/**
  * @brief 	Escritura por I2C1
  *
  * @param 	addr - Direccion de slave
  * @param 	pData - Puntero del buffer de data
  * @param 	size - length de datos a escribir desde el buffer
  * @param 	timeout - Timeout de espera de escritura de datos
  * @retval I2C_HandleTypeDef puntero de i2c_1
  */
HAL_StatusTypeDef I2C1_Write(uint16_t addr, uint8_t* pData, uint16_t size, uint32_t timeout);


/**
  * @brief 	Lectura por I2C1
  *
  * @param 	addr - Direccion de slave
  * @param 	pData - Puntero del buffer de data
  * @param 	size - length de datos a leer en el buffer
  * @param 	timeout - Timeout de espera de lectura de datos
  * @retval I2C_HandleTypeDef puntero de i2c_1
  */
HAL_StatusTypeDef I2C1_Read(uint16_t addr, uint8_t* pData, uint16_t size, uint32_t timeout);


#endif /* API_INC_API_I2C_1_H_ */
