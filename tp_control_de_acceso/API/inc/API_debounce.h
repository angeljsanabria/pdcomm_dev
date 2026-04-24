/*
 * API_debounce.h
 *
 *  Created on: 26 mar 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include "API_delay.h"

/**
 * @brief	Inicio de la FSM/MEF de antirebote
 * @param
 * @param
 */
void debounceFSM_init(void);		// debe cargar el estado inicial


/**
 * @brief 	Actulizacion de la FSM/MEF de antirebote
 * 			Lee pin de entrada para transicion de estados
 * 			y actualizacion de salidas
 * @param
 * @param
 */
void debounceFSM_update(void);


/**
 * @brief Consulta si hubo una pulsacion detectecada por la FSM/MEF
 *        si se llama, se consume el evento y se limpia el flag interno.
 * @retval true  Si habia evento se consume y se limpia el flag interno.
 * @retval false Si no habia evento pendiente.
 */
bool_t readKey(void);


/**
  * @brief  Se detecta el boton de la FSM/FEM presionado
  *         Funcionamiento de Callback: Se prende el led 3 (LED_GREEN) como output visible
  * @retval None
  */
void buttonPressed(void);


/**
  * @brief  Se detecta el boton de la FSM/FEM liberado
  *         Funcionamiento de Callback: Se apaga el led 3 (LED_GREEN) como output visible
  * @retval None
  */
void buttonReleased(void);


#endif /* API_INC_API_DEBOUNCE_H_ */
