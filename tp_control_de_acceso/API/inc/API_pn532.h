/*
 * api_pn532.h
 *
 *  Created on: 17 abr 2026
 *      Author: angel-dev
 */

#ifndef API_INC_API_PN532_H_
#define API_INC_API_PN532_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef bool bool_t;


/* 7 bits segun NXP (modulos tipo Adafruit/Seed). Si tu PCB usa otro valor, medilo. */
#define PN532_I2C_ADDRESS_7BIT		0x24
/* HAL: direccion de hoja de datos desplazada 1 bit a la izquierda */
#define PN532_I2C_ADDR_SHIFT		(0x24 << 1)
#define PN532_CONFIG_MODE_IN_LIST_PASSIVE_TARGET 		0x4A	// Config InListPassiveTarget
#define PN532_CONFIG_MODE_IN_AUTO_POLL				 	0x60
#define PN532_CONFIG_DEFAULT_TIMEOUT				 	500 // en ms

#define PN532_MAX_UID_BUFFER		10
#define PN532_READ_ACK_BUFFER_LEN	64

// DEFINICIONES DE PROTOCOLO
#define PN532_PROTOCOL_READY_BYTE		0x01		// Seguir leyendo la trama
#define PN532_PROTOCOL_STOP_BYTE		0x00		// No seguir leyendo la trama - Enviar Stop
#define PN532_PROTOCOL_00_BYTE		0x00		// Estado de la trama
#define PN532_PROTOCOL_ACK_BYTE		0xFF		// ACK de la trama

#define PN532_PROTOCOL_PREAMBLE_FRAME_1	0x00		// Preambulo de la trama byte 1 de 3
#define PN532_PROTOCOL_PREAMBLE_FRAME_2	0x00		// Preambulo de la trama byte 2 de 3
#define PN532_PROTOCOL_PREAMBLE_FRAME_3	0xFF		// Preambulo de la trama byte 3 de 3
#define PN532_PROTOCOL_ACK_FRAME_BYTE_1	0x00		// ACK trama byte 1 de 3
#define PN532_PROTOCOL_ACK_FRAME_BYTE_2	0xFF		// ACK trama byte 2 de 3
#define PN532_PROTOCOL_ACK_FRAME_BYTE_3	0x00		// ACK trama byte 3 de 3
#define PN532_PROTOCOL_SAMCONFIG_BYTE_1	0x14
/* Modo Normal SAM (NXP 7.2.10): sin SAM en el bus; no es "polling" de tarjeta. */
#define PN532_SAM_MODE_NORMAL			0x01
#define PN532_PROTOCOL_CONFIG_POLLING		PN532_SAM_MODE_NORMAL
#define PN532_PROTOCOL_TFI_HOST_TO_PN532		0xD4		// TFI - Identificado de trama host a PN532
#define PN532_PROTOCOL_TFI_PN532_TO_HOST		0xD5		// TFI - Identificado de trama PN532 a host
#define PN532_PROTOCOL_D5_BYTE		0xD5		// TFI de la trama
#define PN532_PROTOCOL_15_BYTE		0x15		// Codigo de respuesta de la trama
#define PN532_PROTOCOL_DCS_BYTE		0xFD		// Checksum de la trama
#define PN532_PROTOCOL_CMD_INLIST_PASSIVE	0x4B  // CMD InListPassiveTarget
#define PN532_PROTOCOL_LCS_16BITS_VALIDADOR	0x100


/**
  * @brief 	Estados de retorno de las funciones de PN532
  */
typedef enum {
    PN532_OK = 0,
    PN532_ERR_NO_CARD,
    PN532_ERR_I2C,
    PN532_ERR_TIMEOUT,
    PN532_ERR_PROTOCOL,
    PN532_ERR_OTRO,
} PN532_Status_t;

/**
  * @brief 	Modos de trabajo del modulo
  *
  * @param 	pn532_mode_InListPassiveTarget Lectura pasiva: Configura el modulo para esperar
  * 		la orden de lectura. (se debe respetar un tiempo de espera antes de consultar si
  * 		hay dato valido)
  * @param 	pn532_mode_InAutoPoll Lectura activa continua: Configura el modulo en lectura continua
  */
typedef enum
{
	pn532_mode_InListPassiveTarget   	= PN532_CONFIG_MODE_IN_LIST_PASSIVE_TARGET,
	pn532_mode_InAutoPoll    			= PN532_CONFIG_MODE_IN_AUTO_POLL,
} PN532_CONFIG_MODE_T;


/**
  * @brief 	Inicializa el unico PN532_Handler_t interno (valores por defecto).
  * @param 	modo - Modo de lectura (InListPassiveTarget o InAutoPoll).
  *         El desarrollo esta solo para el modo InListPassiveTarget.
  * @retval PN532_OK o PN532_ERR_OTRO si el modo no es valido.
  */
PN532_Status_t PN532_init_module(PN532_CONFIG_MODE_T modo);

/**
  * @brief 	SAM Normal: camino feliz Ready -> ACK 6B -> Ready -> respuesta D5 15 00. read_mode InListPassiveTarget.
  */
PN532_Status_t PN532_config_module(void);

/**
  * @brief Envio por I2C del comando InListPassiveTarget (sin leer respuesta).
  */
PN532_Status_t PN532_polling_send(void);

/**
  * @brief Lee y parsea la respuesta InListPassiveTarget (tras PN532_polling_send y espera).
  */
PN532_Status_t PN532_read_inlist_response(void);

/**
  * @brief Copia el ultimo UID leido por PN532_read_inlist_response (si card_present).
  */
//PN532_Status_t PN532_save_read_uid_card(uint8_t *uid, uint8_t *len);	// DEPRECADO por el que guarda en Char

/**
  * @brief 	Ultimo UID como texto hex ASCII mayusculas contiguo (p. ej. 4 bytes UID -> 8 caracteres + NUL).
  * @param 	out Buffer destino
  * @param 	out_cap Length del buffer out en bytes (incluye el NUL). No es uid_len ni len_card UART.
  * @retval PN532_OK si cabe y uid_len soportado; PN532_ERR_OTRO si buffer chico o uid_len distinto de 4.
  */
PN532_Status_t PN532_save_read_uid_hex(uint8_t *out, uint8_t out_cap);

/**
  * @brief 	Lee y valida trama ACK (buffer interno). PN532_OK si ACK valido, error de protocolo o I2C si no.
  */
PN532_Status_t PN532_read_ack(void);

#endif /* API_INC_API_PN532_H_ */
