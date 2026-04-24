/*
 * API_delay.h
 *
 *  Created on: 19 mar 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

#include <stdint.h>
#include <stdbool.h>


typedef uint32_t tick_t;
typedef bool bool_t;

/**
 * @brief Estructura de control para delays no bloqueantes.
 */
typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;


/**
 * @brief Inicializa una estructura de delay.
 * @param delay Puntero a la estructura delay_t a inicializar.
 * @param duration Duración del delay en ticks.
 */
void delayInit( delay_t * delay, tick_t duration );

/**
 * @brief Comprueba si el tiempo del delay ha expirado.
 * @param delay Puntero a la estructura de delay.
 * @return bool_t Retorna true si el tiempo expiró, false en caso contrario.
 */
bool_t delayRead( delay_t * delay );

/**
 * @brief Actualiza la duración de un delay existente.
 * @param delay Puntero a la estructura de delay.
 * @param duration Nueva duración en ticks.
 */
void delayWrite( delay_t * delay, tick_t duration );

/**
 * @brief Devuelve si el delay existente esta corriendo
 * @param delay Puntero a la estructura de delay.
 * @return bool_t Retorna true si el delay esta corriendo, false en caso contrario.
 */
bool_t delayIsRunning(delay_t * delay);

/**
 * @brief Forzado de stop al delay
 * @param delay Puntero a la estructura de delay.
 * @return void
 */
void delayStop(delay_t * delay);
#endif /* API_INC_API_DELAY_H_ */
