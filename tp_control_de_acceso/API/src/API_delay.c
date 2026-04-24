/*
 * API_delay.c
 *
 *  Created on: 19 mar 2026
 *      Author: angel-dev
 */
#include "API_delay.h"
#include "stm32l4xx_hal.h"

// Funciones de delay no bloqueante
void delayInit( delay_t * delay, tick_t duration ){
	if(delay == NULL) 	return;

	delay->running = false;
	delay->duration = duration;
	delay->startTime = 0;

	return;
}

bool_t delayRead( delay_t * delay ){
	if(delay == NULL) 	return false;

	if(delay->running){
		if((HAL_GetTick() - delay->startTime) >= delay->duration){
			delay->running = false;
			return true;
		}
	}else{
		delay->running = true;
		delay->startTime = HAL_GetTick();
	}

	return false;
}

void delayWrite( delay_t * delay, tick_t duration ){
	if(delay == NULL) 	return;

	delay->duration = duration;

	return;
}

bool_t delayIsRunning(delay_t * delay){
	if(delay == NULL) 	return false;

	return delay->running;
}

void delayStop(delay_t * delay){
	if(delay == NULL) 	return;

	delay->running = false;
	return;
}
