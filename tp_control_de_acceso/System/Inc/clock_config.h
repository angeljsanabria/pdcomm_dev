/**
 ******************************************************************************
 * @file    clock_config.h
 * @brief   System Clock Configuration
 * @details This file provides functions to configure and get system clock
 *          frequencies.
 ******************************************************************************
 */

#ifndef CLOCK_CONFIG_H
#define CLOCK_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Clock Frequencies (will be configured to 120MHz) */
#define SYSTEM_CLOCK_HZ        120000000UL
#define HCLK_FREQ_HZ           120000000UL
#define APB1_FREQ_HZ           60000000UL
#define APB2_FREQ_HZ           120000000UL

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Clock_Config(void);
uint32_t Clock_GetSystemClock(void);
uint32_t Clock_GetHCLK(void);
uint32_t Clock_GetAPB1(void);
uint32_t Clock_GetAPB2(void);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CLOCK_CONFIG_H */
