/*
 * API_cmdDataparser.h
 *
 *  Created on: 8 abr 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_CMD_DATA_PARSER_H_
#define API_INC_API_CMD_DATA_PARSER_H_

#include "API_uart_data.h"

#define CMD_TERMINADOR			ENDSTR
#define CMD_NO_INIT_1			'#'
#define CMD_NO_INIT_2			'/'
#define CMD_MAX_LINE 			64 		// incluye '\0'
#define CMD_MAX_TOKENS  		3 		// COMANDO + maximo 2 argumentos

/**
  * @brief Retornos de proceso de comandos
  */
typedef enum{
	CMD_OK	= 0,
	CMD_ERR_OVERFLOW,
	CMD_ERR_SYNTAX,
	CMD_ERR_UNKNOW,
	CMD_ERR_ARG
} cmd_status_t;

/**
 * @brief 	Incializ la fsm del parser de comandos
 *
 * @param
 * @param
 */
void cmdDataParserInit(void);


/**
 * @brief 	Maquinas de estados del parser.
 * 			debe ser llamamda periodicamente desde el bucle
 * 			Procesa hasta 16 bytes por invocacion (no bloqueante)
 *
 * @param
 * @param
 */
void cmdDataPoll(void);



#endif /* API_INC_API_CMD_DATA_PARSER_H_ */
