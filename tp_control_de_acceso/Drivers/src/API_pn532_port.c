/*
 * API_pn532_port.c
 *
 * Funciones de bajo nivel usando la HAL correspondiente:
 * - I2C via API_i2c_1.
 * - SPI No desarrollado aun.
 */

#include "API_pn532_port.h"
#include "API_i2c_1.h"
#include "stm32l4xx_hal.h"

PN532_PortStatus_t pn532_port_read(PN532_Device_t dev, uint16_t addr_module, uint8_t *data, uint16_t len,
		uint32_t timeout_ms)
{
	switch (dev) {
	case PN532_DEV_I2C:
		if (I2C1_Read(addr_module, data, len, timeout_ms) != HAL_OK) {
			return PN532_PORT_ERR;
		}
		return PN532_PORT_OK;
	case PN532_DEV_SPI:
		return PN532_PORT_ERR;
	default:
		return PN532_PORT_ERR;
	}
}

PN532_PortStatus_t pn532_port_write(PN532_Device_t dev, uint16_t addr_module, const uint8_t *data, uint16_t len,
		uint32_t timeout_ms)
{
	switch (dev) {
	case PN532_DEV_I2C:
		if (I2C1_Write(addr_module, (uint8_t *)data, len, timeout_ms) != HAL_OK) {
			return PN532_PORT_ERR;
		}
		return PN532_PORT_OK;
	case PN532_DEV_SPI:
		return PN532_PORT_ERR;
	default:
		return PN532_PORT_ERR;
	}
}
