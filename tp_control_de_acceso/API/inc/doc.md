Estructura de código
	Se adoptó una arquitectura en cuatro bloques: BSP, Drivers (capa de acceso al periférico), API (servicios reutilizables por dispositivo/protocolo) y APP (lógica de la aplicación).

BSP (Board Support Package): Capa más cercana al hardware: mapeo de pines, instancias (p. ej. GPIO del puerto B en pines 10–12).
Drivers: Funciones reutilizables y orientadas al periférico:: lecturas/escrituras I2C, escritura UART por cadena terminada en NULL o por buffer con longitud, y tambien lecturas. [MEJORA: 'periférico::' → un solo ':'. MEJORA: 'tambien' → 'también'.]
API: Servicios de dispositivo y port. Ejemplo PN532: configuración, armado de tramas, interpretación de respuestas. Junto a ello, el módulo port del PN532 (`API_pn532_port`) sirve para fijar qué tipo de periférico de enlace se usa (I2C o SPI) y desacopla las transferencias bajas hacia el hardware; la APP pasa ese tipo al iniciar el driver.
APP: Lógica de negocio y casos de uso: máquina de estados principal, decisión de accesos, etc.


Driver UART (Drivers/inc/API_uart_data.h, Drivers/src/API_uart_data.c):

	Este módulo encapsula el uso de la USART3 como interfaz serie mediante la HAL de STM32. Mantiene un UART_HandleTypeDef estático para la instancia USART3 (no expone getter del handler, a diferencia del I2C).

Modo: transmisión y recepción por HAL_UART_Transmit / HAL_UART_Receive (operación bloqueante con timeout).
Periférico: USART3.
Velocidad de línea: 57 600 baud (8N1: 8 bits, sin paridad, 1 bit de parada; oversampling 16, sin control de flujo por hardware).
Formato / límites lógicos: tramas de datos acotadas por UART_DATA_MAX_SIZE (256); en transmisión por cadena se usa strlen (cadena terminada en \0); la recepción por tamaño usa un timeout mínimo UART_DATA_MIN_RX_TIMEOUT (10 ms).
Mapeo físico: PD8 TX, PD9 RX, función alternativa AF7 (BSP: VCP / ST-LINK en NUCLEO-L4R5ZI).
Tipo de uso: Polled driver.
API expuesta:
API_uart_data_init
uartSendString
uartSendStringSize
uartReceiveStringSize
uartSendCardToAuthorize
API_uart_data_test
NOTA: La descripción de cada función está en el .h del archivo correspondiente en el repositorio

Driver SPI :

NOTA: No soportado para este proyecto.
API PN532 (API/inc/API_pn532.h, API/src/API_pn532.c):

	Capa de servicio de dispositivo sobre el protocolo PN532. Mantiene un manejador interno (PN532_Handler_t, instancia pn532) con: el tipo de periférico de enlace o bus hacia el chip — `dispositivo` de tipo PN532_Device_t (I2C o SPI) —, los timeouts de operación, la presencia de tarjeta o tag leída y el UID; la lógica arma y reconoce las tramas requeridas (ejemplo: frames de configuración y polling).

Dirección de esclavo en 7 bits según módulo/datasheet: PN532_ADDRESS_7BIT (0x24). (Para acceso vía I2C al HAL, el manejador usa “addr_module” con el valor desplazado, PN532_I2C_ADDR_SHIFT.)
Modo de uso previsto: InListPassiveTarget (lectura bajo demanda); InAutoPoll (no desarrollado).
API expuesta): [MEJORA: título: quitar ')' y dejar 'API expuesta:' como en otras secciones.]
PN532_init_module
PN532_config_module
PN532_polling_send
PN532_read_inlist_response
PN532_read_ack
PN532_save_read_uid_hex
Tipo de uso: polled (transferencias bloqueantes). El funcionamiento está restringido al bus I2C. El SPI está en desarrollo y devuelve error. Errores de la API: PN532_Status_t (Ejemplo: sin tarjeta, error de enlace, *timeout*, protocolo u otro).

NOTA: Detalle de cada función en el .h del repositorio.

Port PN532 (API/inc/API_pn532_port.h, API/src/API_pn532_port.c):

	Capa *specific-port*: abstrae el enlace físico hacia el chip. El servicio API_pn532 no invoca el HAL del I2C ni el periférico directo: todas las transferencias de bytes pasan por pn532_port_read / pn532_port_write. Encapsula el uso de Drivers/inc/API_i2c_1.h (instancia I2C1) cuando el enlace es I2C; no mantiene handle propio (las primitivas I2C1_* reciben dirección 8b y timeout).

Modo: transferencias bloqueantes (misma política *polled* que el I2C de proyecto bajo el HAL). Según el primer parámetro (PN532_Device_t): con PN532_DEV_I2C delega en I2C1_Read / I2C1_Write; con PN532_DEV_SPI devuelve PN532_PORT_ERR (no desarrollado).
Enlace: I2C1 (firmware actual); SPI reservado.
Rol: dirección de esclavo y buffer/len/timeout alineados con la firma del manejador interno del servicio (addr_module, etc.).
Resultado de bajo nivel: PN532_PortStatus_t (PN532_PORT_OK / PN532_PORT_ERR) mapeado a PN532_Status_t en el driver. El tipo de bus PN532_Device_t vive en este .h (incluido por API_pn532.h).
Tipo de uso: *Polled* port; restricción efectiva a bus I2C; SPI con error inmediato.
API expuesta:
pn532_port_read
pn532_port_write
NOTA: La descripción de cada función está en el .h del archivo correspondiente en el repositorio

[MEJORA: En secciones que añadas aparte (p. ej. Driver I2C, APP): en I2C usar el nombre I2C1_Init como en el .h; en la lista de constantes de la APP, cerrar el paréntesis en (500 ms) o unificar en una sola línea; alinear acceso_push_authorization sin espacio inicial.]