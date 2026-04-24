/*
 * API_pn532.c
 *
 *  Created on: 17 abr 2026
 *      Author: angel-dev
 */
#include "API_pn532.h"
#include <stdio.h>
#include <string.h>
#include "API_i2c_1.h"

/**
  * @brief 	estructura principal de control del modulo PN532
  *
  * @param 	i2c_addr_hal - Direccion I2C en formato  con shift
  * @param 	op_timeout_ms - Timeout maximo de operaciones con el PN532 (ms).
  * @param 	card_present- Indica si hay tarjeta/tag detectado.
  * @param 	uid_len - Cantidad de bytes validos del UID.
  * @param 	uid - Almacena el UID del tag (hasta PN532_MAX_UID_BUFFER bytes)
  */
typedef struct {
    uint16_t i2c_addr_hal;
    uint32_t op_timeout_ms;
    bool     card_present;
    uint8_t  uid_len;
    uint8_t  uid[PN532_MAX_UID_BUFFER];
    uint8_t  read_mode;
} PN532_Handler_t;


/*
 * SAMConfiguration minima (0x14, modo Normal 0x01). Trama completa minima.
 */
//static const uint8_t PN532_SAM_NORMAL_FRAME[] = {
//	0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4, 0x14, 0x01, 0x17, 0x00
//};
static const uint8_t PN532_SAM_NORMAL_FRAME[] = {
	PN532_PROTOCOL_PREAMBLE_FRAME_1, PN532_PROTOCOL_PREAMBLE_FRAME_2, PN532_PROTOCOL_PREAMBLE_FRAME_3,
	0x05, 0xFB,
	PN532_PROTOCOL_TFI_HOST_TO_PN532,
	PN532_PROTOCOL_SAMCONFIG_BYTE_1,
	PN532_PROTOCOL_CONFIG_POLLING,
    0x32,                       // 100 ms
    0x01,//0x00,                       // IRQ OFF
    0xE4,//0xE5,                       // DCS
    0x00
};

/*
 * Poll de lectura de tarjeta. Trama completa minima.
 * Prende la antena y espera a leer 1 tarjeta maximo. (timeout por defecto)
 */
static const uint8_t PN532_POLLING_READ[] = {
	PN532_PROTOCOL_PREAMBLE_FRAME_1, PN532_PROTOCOL_PREAMBLE_FRAME_2, PN532_PROTOCOL_PREAMBLE_FRAME_3,
    0x04, 0xFC,
	PN532_PROTOCOL_TFI_HOST_TO_PN532,
	PN532_CONFIG_MODE_IN_LIST_PASSIVE_TARGET,
	0x01, 	// 1 Sola tarjeta
	0x00,
    0xE1,
    0x00	// 106 kbps (ISO14443A - MIFARE - tarjetas blancas )
};

static PN532_Handler_t pn532;

PN532_Status_t PN532_init_module(PN532_CONFIG_MODE_T modo)
{
	pn532.i2c_addr_hal = PN532_I2C_ADDR_SHIFT;
	pn532.op_timeout_ms = PN532_CONFIG_DEFAULT_TIMEOUT;
	pn532.card_present = false;
	pn532.uid_len = 0;
	if (modo == pn532_mode_InListPassiveTarget) {
		pn532.read_mode = pn532_mode_InListPassiveTarget;
	} else if (modo == pn532_mode_InAutoPoll) {
		pn532.read_mode = pn532_mode_InAutoPoll;
	} else {
		return PN532_ERR_OTRO;
	}
	(void)memset(pn532.uid, 0x00, (size_t)PN532_MAX_UID_BUFFER);
	return PN532_OK;
}

PN532_Status_t PN532_read_ack(void)
{
	uint8_t buff_rx[PN532_READ_ACK_BUFFER_LEN] = {0};
	uint8_t n;
	uint8_t i;
	uint8_t found;
	uint8_t cola_pendiente_i2c;

	n = 0;
	found = 0;
	cola_pendiente_i2c = 1;
//	while (n < PN532_READ_ACK_BUFFER_LEN) {
//		if (I2C1_Read(pn532.i2c_addr_hal, &buff_rx[n], 1, pn532.op_timeout_ms) != HAL_OK) {
//			return PN532_ERR_I2C;
//		}
//		n++;
//		if (n < 3) {
//			continue;
//		}
//		i = (uint8_t)(n - 1);
//		if ((buff_rx[i - 2] == PN532_PROTOCOL_PREAMBLE_FRAME_1)
//		    && (buff_rx[i - 1] == PN532_PROTOCOL_PREAMBLE_FRAME_2)
//		    && (buff_rx[i] == PN532_PROTOCOL_PREAMBLE_FRAME_3)) {
//			found = 1;
//			break;
//		}
//	}
	/* Si solo hubo 0x01 byte a byte, un unico burst suele traer la trama real. */
	if (found == 0) {
		if (I2C1_Read(pn532.i2c_addr_hal, buff_rx, 7,
			      pn532.op_timeout_ms) != HAL_OK) {
			return PN532_ERR_I2C;
		}
		n = 0;
		while (n < PN532_READ_ACK_BUFFER_LEN) {
			n++;
			if (n < 3) {
				continue;
			}
			i = (uint8_t)(n - 1);
			if ((buff_rx[i - 2] == PN532_PROTOCOL_PREAMBLE_FRAME_1)
			    && (buff_rx[i - 1] == PN532_PROTOCOL_PREAMBLE_FRAME_2)
			    && (buff_rx[i] == PN532_PROTOCOL_PREAMBLE_FRAME_3)) {
				found = 1;
				cola_pendiente_i2c = 0;
				break;
			}
		}
	}
	if (found == 0) {
		return PN532_ERR_PROTOCOL;
	}

	if ((uint8_t)(n + 3) > PN532_READ_ACK_BUFFER_LEN) {  // No tengo espacio para la respuesta ack
		return PN532_ERR_PROTOCOL;
	}

	if (cola_pendiente_i2c != 0) {
		if (I2C1_Read(pn532.i2c_addr_hal, &buff_rx[n], 3, pn532.op_timeout_ms) != HAL_OK) {
			return PN532_ERR_I2C;
		}
	}

	n = (uint8_t)(n + 3);
	if ((buff_rx[n - 3] != PN532_PROTOCOL_ACK_FRAME_BYTE_1)
	    || (buff_rx[n - 2] != PN532_PROTOCOL_ACK_FRAME_BYTE_2)
	    || (buff_rx[n - 1] != PN532_PROTOCOL_ACK_FRAME_BYTE_3)) {
		return PN532_ERR_PROTOCOL;
	}

	return PN532_OK;
}

static PN532_Status_t PN532_config_module_normal(void)
{
	if (I2C1_Write(pn532.i2c_addr_hal,
		       (uint8_t *)PN532_SAM_NORMAL_FRAME,
		       (uint16_t)sizeof(PN532_SAM_NORMAL_FRAME),
		       pn532.op_timeout_ms) != HAL_OK) {
		return PN532_ERR_I2C;
	}

	return PN532_OK;
}


PN532_Status_t PN532_config_module(void)
{
	// Solo config de lectura tipo polling
	return PN532_config_module_normal();
}


/*
 * Respuesta InListPassiveTarget desde PN532_PROTOCOL_TFI_PN532_TO_HOST
 * y comando
 */
PN532_Status_t PN532_read_inlist_response(void)
{
	uint8_t buff_rx[PN532_READ_ACK_BUFFER_LEN];
	uint16_t p;
	uint16_t scan;
	uint8_t found;
	uint8_t len_field;
	uint8_t count_cards;
	uint8_t uid_len;
	uint16_t sum;
	uint8_t k;

	(void)memset(buff_rx, 0, sizeof(buff_rx));

	if (I2C1_Read(pn532.i2c_addr_hal, buff_rx, PN532_READ_ACK_BUFFER_LEN,
		      pn532.op_timeout_ms) != HAL_OK) {
		return PN532_ERR_I2C;
	}

	/*
	 * Buscar 00 00 FF de trama de datos: LEN+LCS==0x100, TFI D5, CMD 4B.
	 * Si antes viene solo ACK (00 00 FF 00 FF 00), LEN+LCS no suma 0x100: seguir buscando.
	 * Ejemplo valido: 01 | 00 00 FF 0C F4 D5 4B 01 01 ... (0x01 listo I2C opcional).
	 */
	found = 0;
	p = 0;
	for (scan = 0; (scan + 2) < PN532_READ_ACK_BUFFER_LEN; scan++) {
		if ((buff_rx[scan] == PN532_PROTOCOL_PREAMBLE_FRAME_1)
		    && (buff_rx[scan + 1] == PN532_PROTOCOL_PREAMBLE_FRAME_2)
		    && (buff_rx[scan + 2] == PN532_PROTOCOL_PREAMBLE_FRAME_3)) {
			if ((uint16_t)(scan + 6) >= PN532_READ_ACK_BUFFER_LEN) {
				continue;
			}
			if ((uint16_t)(buff_rx[scan + 3] + buff_rx[scan + 4])
			    != (uint16_t)PN532_PROTOCOL_LCS_16BITS_VALIDADOR) {
				continue;
			}
			if ((buff_rx[scan + 5] == PN532_PROTOCOL_TFI_PN532_TO_HOST)
			    && (buff_rx[scan + 6] == PN532_PROTOCOL_CMD_INLIST_PASSIVE)) {
				found = 1;
				p = scan;
				break;
			}
		}
	}

	if (found == 0) {
		return PN532_ERR_PROTOCOL;
	}

	if ((uint16_t)(p + 5) >= PN532_READ_ACK_BUFFER_LEN) {
		return PN532_ERR_PROTOCOL;
	}

	len_field = buff_rx[p + 3];
	/* LEN = TFI + PD0..PDn (sin DCS). DCS va despues; checksum = sum(TFI..PDn+DCS) mod 256 == 0. */
	if ((uint16_t)(p + 5 + (uint16_t)len_field) >= PN532_READ_ACK_BUFFER_LEN) {
		return PN532_ERR_PROTOCOL;
	}

	count_cards = buff_rx[p + 7];
	if (count_cards == 0) {
		pn532.card_present = false;
		pn532.uid_len = 0;
		(void)memset(pn532.uid, 0, sizeof(pn532.uid));
		return PN532_ERR_NO_CARD;
	}

	/* Tras D5 4B: NbTg, Tg, ATQA(2), SAK, longitud UID, UID (NXP / mismo mapa que Adafruit). */
	if ((uint16_t)(p + 13) >= PN532_READ_ACK_BUFFER_LEN) {
		return PN532_ERR_PROTOCOL;
	}

	uid_len = buff_rx[p + 12];
	if (uid_len == 0 || uid_len > PN532_MAX_UID_BUFFER) {
		return PN532_ERR_PROTOCOL;
	}

	if ((uint16_t)(p + 13 + (uint16_t)uid_len) > PN532_READ_ACK_BUFFER_LEN) {
		return PN532_ERR_PROTOCOL;
	}

	(void)memcpy(pn532.uid, &buff_rx[p + 13], uid_len);
	pn532.uid_len = uid_len;
	pn532.card_present = true;

	sum = 0;
	for (k = 0; k <= len_field; k++) {
		sum = (uint16_t)(sum + buff_rx[p + 5 + k]);
	}

	if ((sum & 0xFF) != 0) {
		return PN532_ERR_PROTOCOL;
	}

	return PN532_OK;
}

PN532_Status_t PN532_polling_send(void)
{
	if (I2C1_Write(pn532.i2c_addr_hal,
		       (uint8_t *)PN532_POLLING_READ,
		       (uint16_t)sizeof(PN532_POLLING_READ),
		       pn532.op_timeout_ms) != HAL_OK) {
		return PN532_ERR_I2C;
	}

	return PN532_OK;
}


//PN532_Status_t PN532_save_read_uid_card(uint8_t *uid, uint8_t *len)		// Deprecado, porque copiaba en bytes.
//{
//	if ((uid == NULL) || (len == NULL)) {
//		return PN532_ERR_OTRO;
//	}
//
//	if (pn532.card_present == false) {
//		return PN532_ERR_NO_CARD;
//	}
//
//	if(pn532.uid_len > PN532_MAX_UID_BUFFER){
//		return PN532_ERR_OTRO;
//	}
//
//	*len = pn532.uid_len;
//	(void)memcpy(uid, pn532.uid, (size_t)pn532.uid_len);
//	return PN532_OK;
//}

PN532_Status_t PN532_save_read_uid_hex(uint8_t *out, uint8_t out_cap)
{
	int w;

	if (out == NULL) {
		return PN532_ERR_OTRO;
	}
	if (out_cap == 0) {
		return PN532_ERR_OTRO;
	}

	if (pn532.card_present == false) {
		return PN532_ERR_NO_CARD;
	}

	if (pn532.uid_len > PN532_MAX_UID_BUFFER) {
		return PN532_ERR_OTRO;
	}

	if (pn532.uid_len == 0) {
		out[0] = '\0';
		return PN532_OK;
	}

	if (out_cap < ((size_t)pn532.uid_len * 2)+ 1) {
		return PN532_ERR_OTRO;
	}

	switch (pn532.uid_len) {
		// Agregar otros si hacen falta
		case 4:
			w = snprintf((char*)out, (size_t)out_cap, "%02X%02X%02X%02X",(uint8_t)pn532.uid[0], (uint8_t)pn532.uid[1], (uint8_t)pn532.uid[2], (uint8_t)pn532.uid[3]);
			break;
		default:
			return PN532_ERR_OTRO;
	}

	if (w < 0 || (size_t)w >= out_cap) {
		return PN532_ERR_OTRO;
	}
	return PN532_OK;
}
