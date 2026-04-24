/**
 ******************************************************************************
 * @file    system_config.h
 * @brief   System Configuration
 * @details This file provides system-wide configuration definitions.
 ******************************************************************************
 */

#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* System Configuration */
#define SYSTEM_CLOCK_HZ        120000000UL

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_CONFIG_H */
