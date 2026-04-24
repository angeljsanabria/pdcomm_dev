/*
 * API_acceso_fsm.c
 *
 *  Created on: 18 abr 2026
 *      Author: angel-dev
 */

#include <APP_acceso_fsm.h>
#include <string.h>

#include "API_accesos_output.h"
#include "API_uart_data.h"
#include "bsp_button.h"

enum accesso_pendiente{
	_no_esperando 			= 0,
	_esperando_autorizacion	= 1,
};

/*
 * Estructura principal de control del acceso
 */
static acceso_t acceso;

void acceso_push_authorization(accesoExec_t exe){
	if(!acceso.pending_authorization)	return;

	if(acceso.exe_authorization == ACCESO_EXEC_PENDIENTE){
		acceso.exe_authorization = exe;
	}
}

void acceso_FSM_init(void)
{
	(void)memset(&acceso, 0, sizeof(acceso));
	acceso.st_actual = ACCESO_FSM_INIT;
	delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
}

void acceso_FSM_update(void)
{
	switch (acceso.st_actual) {
	case ACCESO_FSM_INIT:
		if (!delayIsRunning(&acceso.delay_fsm)) {
			acceso.retry_count = 0;
			acceso.pending_authorization = _no_esperando;
			acceso.exe_authorization = ACCESO_EXEC_PENDIENTE;
			acceso.valid_card = 0;
			acceso.len_card = 0;
			(void)memset(acceso.value_card, 0, sizeof(acceso.value_card));
			acceso.lastStNFC = PN532_init_module(pn532_mode_InListPassiveTarget);
			if (acceso.lastStNFC != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
				break;
			}
		}
		acceso_cerrar_todos();
		if (delayRead(&acceso.delay_fsm)) {
			delayStop(&acceso.delay_fsm);
			acceso.st_actual = ACCESO_FSM_CONFIG_PN532;
		}
		break;

	case ACCESO_FSM_CONFIG_PN532:
		if (!delayIsRunning(&acceso.delay_fsm)) {
			acceso.lastStNFC  = PN532_config_module();
			if (acceso.lastStNFC  != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
				break;
			}
			delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
		}
		if (delayRead(&acceso.delay_fsm)) {
			acceso.lastStNFC = PN532_read_ack();

			if (acceso.lastStNFC != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
			} else {
				delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
				acceso.st_actual = ACCESO_FSM_IDLE;
			}
		}
		break;

	case ACCESO_FSM_IDLE:
		if (delayRead(&acceso.delay_fsm)) {
			acceso.st_actual = ACCESO_FSM_POLL_TO_READ_CARD;
		}
		break;

	case ACCESO_FSM_POLL_TO_READ_CARD:
		if (!delayIsRunning(&acceso.delay_fsm)) {
			acceso.lastStNFC = PN532_polling_send();
			if (acceso.lastStNFC != PN532_OK) {
				acceso.st_actual = ACCESO_FSM_IDLE;
				break;
			}
			acceso.retry_count = 0;
			delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
			acceso.st_actual = ACCESO_FSM_POLL_READ_ACK;
		}
		break;

	case ACCESO_FSM_POLL_READ_ACK:
		uint8_t irq_ack;

		irq_ack = BSP_Button_Read();
		if ((delayRead(&acceso.delay_fsm) != 0) || (irq_ack)) {
			acceso.lastStNFC = PN532_read_ack();
			if (acceso.lastStNFC == PN532_OK) {
				acceso.retry_count = 0;
				delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
				acceso.st_actual = ACCESO_FSM_PROCESS_AND_VALIDATE_DATA;
			} else {
				acceso.retry_count++;
				if (acceso.retry_count <= ACCESO_MAX_RETRIES) {
					delayInit(&acceso.delay_fsm, ACCESO_DELAY_TINY_DEFAULT);
				} else {
					acceso.retry_count = 0;
					acceso.st_actual = ACCESO_FSM_IDLE;
				}
			}
		}
		break;

	case ACCESO_FSM_PROCESS_AND_VALIDATE_DATA:
		uint8_t irq_in;

		irq_in = BSP_Button_Read();
		if ((delayRead(&acceso.delay_fsm) != 0) || (irq_in == 0)) {
			acceso.lastStNFC = PN532_read_inlist_response();
			if (acceso.lastStNFC == PN532_OK) {
				acceso.lastStNFC = PN532_save_read_uid_hex(
				    acceso.value_card,
				    (uint8_t)sizeof(acceso.value_card));
				if (acceso.lastStNFC == PN532_OK) {
					acceso.len_card = (uint8_t)strlen((char *)acceso.value_card);
					acceso.valid_card = 1;
					acceso.retry_count = 0;
					delayStop(&acceso.delay_fsm);
					acceso.st_actual = ACCESO_FSM_SENT_TO_AUTHORIZER;
				} else {
					acceso.valid_card = 0;
					acceso.len_card = 0;
					acceso.st_actual = ACCESO_FSM_IDLE;
				}
			} else if (acceso.lastStNFC == PN532_ERR_NO_CARD) {
				acceso.valid_card = 0;
				acceso.len_card = 0;
				(void)memset(acceso.value_card, 0, sizeof(acceso.value_card));
				delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
				acceso.st_actual = ACCESO_FSM_IDLE;
			} else {
				acceso.st_actual = ACCESO_FSM_IDLE;
			}
		}
		break;

	case ACCESO_FSM_SENT_TO_AUTHORIZER:

		if(acceso.valid_card == 0){
			acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
		}
		acceso.pending_authorization = _esperando_autorizacion;
		acceso.exe_authorization = ACCESO_EXEC_PENDIENTE;
		uartSendCardToAuthorize(acceso.value_card, (uint16_t)acceso.len_card);
		delayInit(&acceso.delay_fsm, ACCESO_DELAY_DEFAULT);
		acceso.st_actual = ACCESO_FSM_WAIT_RX;
		break;

	case ACCESO_FSM_WAIT_RX:

		if(acceso.exe_authorization != ACCESO_EXEC_PENDIENTE){
			acceso.st_actual = ACCESO_FSM_EXEC_ACCESO;
			delayStop(&acceso.delay_fsm);
		}
		else if (delayRead(&acceso.delay_fsm)){
			acceso.retry_count++;
			delayStop(&acceso.delay_fsm);
			if (acceso.retry_count <= ACCESO_MAX_RETRIES) {
				acceso.st_actual = ACCESO_FSM_SENT_TO_AUTHORIZER;
			}else{
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
			}
		}

		break;

	case ACCESO_FSM_EXEC_ACCESO:
		if (!delayIsRunning(&acceso.delay_fsm)) {
			acceso.pending_authorization = _no_esperando;
			acceso.retry_count = 0;
			delayWrite(&acceso.delay_fsm, ACCESO_DELAY_EXECUTE);
			if(acceso.exe_authorization == ACCESO_EXEC_ACCESO_A){
				acceso_A_abrir();
			}else if(acceso.exe_authorization == ACCESO_EXEC_ACCESO_B){
				acceso_B_abrir();
			}else if(acceso.exe_authorization == ACCESO_EXEC_NO_AUTORIZADO){
				acceso_denegado_indicador();
			}else{
				acceso.st_actual = ACCESO_FSM_EXEC_ERROR_LED_SEQ;
			}

		}
		if (delayRead(&acceso.delay_fsm)) {
			acceso.st_actual = ACCESO_FSM_INIT;
		}
		break;

	case ACCESO_FSM_EXEC_ERROR_LED_SEQ:
		for(uint8_t loop = 0; loop <= 15; loop++){
			acceso_error_indicador();
			HAL_Delay(50);
		}

		acceso.pending_authorization = _no_esperando;
		acceso.retry_count = 0;
		acceso.st_actual = ACCESO_FSM_IDLE;
		break;
	default:
		acceso.st_actual = ACCESO_FSM_INIT;
		break;
	}
}
