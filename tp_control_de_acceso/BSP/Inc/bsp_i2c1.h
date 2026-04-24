/*
 * bsp_i2c1.h
 *
 *  Created on: 11 abr 2026
 *      Author: angel-dev
 */

#ifndef BSP_I2C1_H_
#define BSP_I2C1_H_

#include "stm32l4xx_hal.h"

/* Mapeo I2C-1
 *	PB8     ------> I2C1_SCL
 *	PB9     ------> I2C1_SDA
 *
 * */
#define BSP_I2C1_SCL_PIN        GPIO_PIN_8
#define BSP_I2C1_SDA_PIN        GPIO_PIN_9
#define BSP_I2C1_PORT     		GPIOB

/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c);


/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c);


#endif /* BSP_I2C1_H_ */
