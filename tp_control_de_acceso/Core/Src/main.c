/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "API_accesos_output.h"
#include "APP_acceso_fsm.h"
#include "APP_cmd_data_parser.h"
#include "main.h"
#include "stdio.h"
#include <stdint.h>
#include "core_cm4.h"   /* Para ITM_SendChar */

#include "bsp.h"
#include "clock_config.h"
#include "API_uart_data.h"
#include "API_uart_data.h"
#include "API_i2c_1.h"

/**
 * @brief Definiciones de patrones y repeticiones de cada uno
 */

int _write(int le, char *ptr, int len)
{
int DataIdx;
for(DataIdx = 0; DataIdx < len; DataIdx++)
	{
	ITM_SendChar(*ptr++);
	}
return len;
}



/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{


	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	// Ver si se esta inciando HAL_InitTick()


	/* Configure the system clock to 120MHz */
	Clock_Config();


	/* Initialize Board Support Package (LEDs, buttons, etc.) */
	BSP_Init();

	/* Inicilizo Perifericos*/
    (void)API_uart_data_init();
    (void)I2C1_Init();

	API_uart_data_test();

	/* Inicilizo FSM de UART*/
	cmdDataParserInit();
	/* Inicilizo FSM principal de acceso*/
	acceso_FSM_init();


	while (1)
	{
		cmdDataPoll();
		acceso_FSM_update();

	}

	return 0;
}



/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
