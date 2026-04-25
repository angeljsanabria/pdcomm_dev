/*
 * API_pn532_port.h
 *
 * Capa de puerto: tipo de bus/interfaz frente al PN532 (I2C, SPI, etc.).
 */

#ifndef API_INC_API_PN532_PORT_H_
#define API_INC_API_PN532_PORT_H_

#include <stdint.h>

/**
  * @brief  Bus o interfaz entre MCU y PN532
  */
typedef enum {
	PN532_DEV_I2C = 0,
	PN532_DEV_SPI = 1,
} PN532_Device_t;

typedef int32_t PN532_PortStatus_t;
#define PN532_PORT_OK	((PN532_PortStatus_t)0)
#define PN532_PORT_ERR	((PN532_PortStatus_t)-1)

/**
  * @param  dev             Tipo de interfaz (I2C o SPI).
  * @param  addr_module     I2C: direccion 8b como espera el HAL. SPI: sin uso por ahora.
  * @param  data            Buffer destino.
  * @param  len             Longitud a leer.
  * @param  timeout_ms      Timeout de la transferencia.
  * @retval PN532_PORT_OK o PN532_PORT_ERR.
  */
PN532_PortStatus_t pn532_port_read(PN532_Device_t dev, uint16_t addr_module, uint8_t *data, uint16_t len,
		uint32_t timeout_ms);

/**
  * @param  dev             Tipo de interfaz (I2C o SPI).
  * @param  addr_module     I2C: direccion 8b. SPI: sin uso por ahora.
  * @param  data            Buffer fuente.
  * @param  len             Longitud a enviar.
  * @param  timeout_ms      Timeout.
  * @retval PN532_PORT_OK o PN532_PORT_ERR.
  */
PN532_PortStatus_t pn532_port_write(PN532_Device_t dev, uint16_t addr_module, const uint8_t *data, uint16_t len,
		uint32_t timeout_ms);

#endif /* API_INC_API_PN532_PORT_H_ */
