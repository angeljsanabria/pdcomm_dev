/*
 * API_acceso_fsm.h
 *
 *  Created on: 18 abr 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_ACCESO_FSM_H_
#define API_INC_API_ACCESO_FSM_H_

#include <stdint.h>

#include "API_pn532.h"
#include "API_delay.h"


#define ACCESO_DELAY_DEFAULT			500
#define ACCESO_DELAY_TINY_DEFAULT		200
#define ACCESO_DELAY_EXECUTE			3000

#define ACCESO_MAX_VALUE_CARD		PN532_MAX_UID_BUFFER

#define ACCESO_MAX_RETRIES			2

/**
  * @brief 	Estados de la FSM de acceso
  */
typedef enum {
	ACCESO_FSM_INIT = 0,
	ACCESO_FSM_CONFIG_PN532,
	ACCESO_FSM_IDLE,
	ACCESO_FSM_POLL_TO_READ_CARD,
	ACCESO_FSM_POLL_READ_ACK,
	ACCESO_FSM_PROCESS_AND_VALIDATE_DATA,
	ACCESO_FSM_SENT_TO_AUTHORIZER,
	ACCESO_FSM_WAIT_RX,
	ACCESO_FSM_EXEC_ERROR_LED_SEQ,
	ACCESO_FSM_EXEC_ACCESO,
} accesoState_t;

typedef enum {
	ACCESO_EXEC_PENDIENTE = 0,
	ACCESO_EXEC_NO_AUTORIZADO,
	ACCESO_EXEC_ACCESO_A,
	ACCESO_EXEC_ACCESO_B,
	ACCESO_EXEC_ERROR,
} accesoExec_t;

/**
  * @brief 	Estructura principal de control de acceso
  * @param 	st_actual - Estado actual de la FSM
  * @param 	delay_fsm - Puntero del delay no bloqueante
  * @param 	pending_authorization - Usado para indicar si esta pendiente de respuesta del autorizador
  * @param 	exe_authorization - Accion a ejecutar
  * @param 	valid_card - Indicador de tarjeta leida valida
  * @param 	len_card - Length de la tarjeta leida
  * @param 	value_card - Buffer de la tarjeta leida transformada a char
  * @param 	retry_count - Timeout maximo de operaciones con el PN532 (ms).
  * @param 	lastStNFC - Retorno de la API del modulo PN532 NFC
  */
typedef struct {
	accesoState_t st_actual;
	delay_t delay_fsm;
	uint8_t pending_authorization;
	accesoExec_t exe_authorization;
	uint8_t valid_card;
	uint8_t len_card;
	uint8_t value_card[ACCESO_MAX_VALUE_CARD];
	uint8_t retry_count;
	PN532_Status_t lastStNFC;
} acceso_t;




/**
 * @brief	Inicio de la FSM/MEF de acceso (FSM PRINCIPAL)
 * @param	Inicializa la maquina de estados y el delay no bloqueante usado
 *
 */
void acceso_FSM_init(void);


/**
 * @brief	Inicio de la FSM/MEF de acceso (FSM PRINCIPAL)
 * 			Configura el modulo lector de tarjetas NFC
 * 			Hace poll de lectura cada 500ms
 * 			Si hay una lectura valida la envia al dispositivo autorizador
 * 			Ejecuta la respuesta del modulo autorizador
 * 			En cualquier caso de error o timeout se indica con la secuencia de error
 * 	@param	None
 * 	@retval	None
 */
void acceso_FSM_update(void);


/**
 * @brief	Recibe un push de accion a ejecutar sobre la estructura del autorizador
 * 			la accion a ejecutar enviada por el autorizador
 * 			solo se acepta si esta esperando respuesta
 * 	@param	None
 * 	@retval	None
 */
void acceso_push_authorization(accesoExec_t exe);

#endif /* API_INC_API_ACCESO_FSM_H_ */
