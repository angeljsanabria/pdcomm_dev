/*
 * API_cmd_data_parser.c
 *
 *  Created on: 8 abr 2026
 *      Author: angel-dev
 */
#include <APP_acceso_fsm.h>
#include <APP_cmd_data_parser.h>
#include <string.h>


/**
  * @brief Comandos soportados
  */
#define CMD_INPUT_ACCESO_A		"ACCESO-A"
#define CMD_INPUT_ACCESO_B		"ACCESO-B"
#define CMD_INPUT_NO_AUTORIZADO	"ACCESO0"



/**
  * @brief Definiciones de ayuda
  */
#define CMD_INPUT_MIN_LENGTH		4
#define MINUSCULA_A					0x61
#define MINUSCULA_Z					0x7A
#define MINUSCULA_TO_MAYUS_OFFSET	0x20

typedef enum{
	CMD_IDLE,
	CMD_RECEIVING,
	CMD_PROCESS,
	CMD_EXEC,
	CMD_ERROR,
	CMD_DISMISS,
} parserCmdState_t;

enum {
	CMD_TO_EXE_NONE		= 0,
	CMD_TO_EXE_PUSH_ACCESO_A,
	CMD_TO_EXE_PUSH_ACCESO_B,
	CMD_TO_EXE_PUSH_NO_AUTORIZADO,
	CMD_TO_EXE_LED_OFF,
	CMD_TO_EXE_LED_TOGGLE,
};

static parserCmdState_t currentSt;
static uint16_t ptrRxBuff;
static uint16_t cmdDataToExe;
static cmd_status_t cmdDataStatus;
static uint8_t lastCharRead;
static uint8_t rxBuff[CMD_MAX_LINE] = {0};


/**
 * @brief 	Lectura de un unico caracteres de la uart de Datos
 *
 * @retval	true si se recibio un caracter
 * @retval	false si no recibio un caracter
 *
 */
static bool_t _readOneChar(void){
	if(uartReceiveStringSize(&rxBuff[ptrRxBuff], 1) == true){
		lastCharRead = rxBuff[ptrRxBuff];
		ptrRxBuff++;
		return true;
	}
	return false;
}

/**
 * @brief	Coloco el '0' para poder usar las funciones de string, reemplazando el retorno de carro
 * 			Aseguro el string en el Input Data
 */
static void _addNullToInputStr(void)
{
	if(ptrRxBuff > 0){
		ptrRxBuff--;
		rxBuff[ptrRxBuff] = 0x00;
	}
}

/**
 * @brief 	trasformo el comando ingresado a mayusculas
 * 			busco si hay algun caracter en minuscula y le aplico el offset
 * 			correspondiente para transformar a su mayuscula
 *
 */
static void _inputCmdToLowerCase(void){
	for (uint16_t loop = 0; loop < ptrRxBuff; loop++){
		if ((rxBuff[loop] >= MINUSCULA_A) && (rxBuff[loop] <= MINUSCULA_Z)){
			rxBuff[loop] =  rxBuff[loop] - (uint8_t)MINUSCULA_TO_MAYUS_OFFSET;
		}
	}
}

/**
 * @brief 	Check de comandos listados en el frame recibido
 *
 * @retval 	cmd_status_t CMD_OK si hay algun comando valido a ejecutar
 */
static cmd_status_t _checkInputCmd(){

	if (strstr((char *)rxBuff, CMD_INPUT_ACCESO_A) != NULL) {
		cmdDataToExe = CMD_TO_EXE_PUSH_ACCESO_A;
		return CMD_OK;
	}

	if (strstr((char *)rxBuff, CMD_INPUT_ACCESO_B) != NULL) {
		cmdDataToExe = CMD_TO_EXE_PUSH_ACCESO_B;
		return CMD_OK;
	}

	if (strstr((char *)rxBuff, CMD_INPUT_NO_AUTORIZADO) != NULL) {
		cmdDataToExe = CMD_TO_EXE_PUSH_NO_AUTORIZADO;
		return CMD_OK;
	}

	return CMD_ERR_SYNTAX;

}



/**
 * @brief 	FUNCIONES PUBLICAS
 * 			documentadas en el .h
 */
void cmdDataParserInit(void){
	currentSt = CMD_IDLE;
	ptrRxBuff = 0;
	lastCharRead = 0;
	cmdDataStatus = CMD_OK;
	cmdDataToExe = CMD_TO_EXE_NONE;
	return;
}



void cmdDataPoll(void){

	switch (currentSt) {
	case CMD_IDLE:
		if(_readOneChar()){
			if((lastCharRead == CMD_TERMINADOR) || (lastCharRead == CMD_NO_INIT_1) || (lastCharRead == CMD_NO_INIT_2)){
				ptrRxBuff = 0;
			}
			else{
				currentSt = CMD_RECEIVING;
			}
		}

		break;

	case CMD_RECEIVING:
		if (ptrRxBuff >= CMD_MAX_LINE) {
			cmdDataStatus = CMD_ERR_OVERFLOW;
			currentSt = CMD_ERROR;
			break;
		}
		if(_readOneChar()){
			if(lastCharRead == CMD_TERMINADOR){
				if(ptrRxBuff <= CMD_INPUT_MIN_LENGTH){
					cmdDataStatus = CMD_ERR_SYNTAX;
					currentSt  = CMD_ERROR;
					break;
				}
				currentSt  = CMD_PROCESS;
				break;
			}
			if(ptrRxBuff >= CMD_MAX_LINE){
				cmdDataStatus = CMD_ERR_OVERFLOW;
				currentSt  = CMD_ERROR;
				break;
			}
		}
		break;

	case CMD_PROCESS:

		_addNullToInputStr();
		_inputCmdToLowerCase();
		cmdDataStatus = _checkInputCmd();
		if(cmdDataStatus == CMD_OK)		currentSt = CMD_EXEC;
		else							currentSt = CMD_ERROR;

		break;

	case CMD_EXEC:
		//
		if(cmdDataToExe == CMD_TO_EXE_PUSH_ACCESO_A){
			acceso_push_authorization(ACCESO_EXEC_ACCESO_A);
		}
		else if(cmdDataToExe == CMD_TO_EXE_PUSH_ACCESO_B){
			acceso_push_authorization(ACCESO_EXEC_ACCESO_B);
		}
		else if(cmdDataToExe == CMD_TO_EXE_PUSH_NO_AUTORIZADO){
			acceso_push_authorization(ACCESO_EXEC_NO_AUTORIZADO);
		}
		else{
			acceso_push_authorization(ACCESO_EXEC_ERROR);
		}



		ptrRxBuff = 0;
		cmdDataToExe = CMD_TO_EXE_NONE;
		currentSt = CMD_IDLE;
		break;

	case CMD_ERROR:

		ptrRxBuff = 0;
		cmdDataToExe = CMD_TO_EXE_NONE;
		currentSt = CMD_IDLE;
		break;

	case CMD_DISMISS:

		ptrRxBuff = 0;
		cmdDataToExe = CMD_TO_EXE_NONE;
		currentSt = CMD_IDLE;
		break;
	default:
		currentSt = CMD_IDLE;
		break;

	}
}
