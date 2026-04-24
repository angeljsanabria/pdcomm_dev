# Release note: 

## Resumen

El proyecto es un sistema de control de acceso con tarjetas NFC con autorizador externo. El firmware en la NUCLEO se encarga de configurar y controlar un módulo lector de tarjetas NFC (PN532) por interfaz I2C. Cada 500ms se envía un comando de lectura al módulo y lee su respuesta. Si hay tarjeta válida en la respuesta, los datos leídos se envían por UART hacia un módulo de interfaz inalámbrica; un sistema embebido externo (Autorizador) recibe este mensaje y válida la tarjeta frente a una tabla de tarjetas autorizadas y el tipo de acceso (acceso A, acceso B o no autorizado). El microcontrolador recibe la respuesta del Autorizador y acciona salidas digitales asociadas al acceso concedido. Luego apaga los indicadores de acceso y vuelve a enviar el comando de lectura de tarjetas para repetir el ciclo de control.

---

## Perifericos

### I2C — PN532

- Configuracion y control del modulo NFC.
- Polling de lectura de tarjetas (orden de magnitud **500 ms** en la MEF / delays del proyecto).
- Verificacion de lectura: presencia de tarjeta y valor de **UID** (validacion y formato para envio).

### UART — XBee 3

- Comunicacion con el modulo **XBee 3** (enlace hacia el autorizador externo).
- Envio de tramas de **solicitud de autorizacion** con el **UID** de la tarjeta NFC.
- Recepcion y **decodificacion** de la respuesta del autorizador.

### GPIO — salidas

- Pines configurados como **salidas digitales** (LEDs vía BSP).
- Indicador de **ACCESO-A**.
- Indicador de **ACCESO-B**.
- Indicador de **NO-ACCESO**.
- Indicador de **error**.

### SysTick / tiempo

- Base para **delays no bloqueantes** (`HAL_GetTick` / capa `delay_t` en la MEF).

### Hardware y pinout (USART3)

| Parametro | Valor |
|-----------|--------|
| Periferico | **USART3** (`huart3`, `USART3`) |
| Placa de referencia (BSP) | **NUCLEO-L4R5ZI** |
| **TX** | **PD8** (AF7 USART3) |
| **RX** | **PD9** (AF7 USART3) |
| Puerto GPIO | **GPIOD** |

---

## Parametros de linea (UART de datos)

| Parametro | Valor en firmware |
|-----------|-------------------|
| Baudios | **57600** |
| Bits de datos | **8** |
| Paridad | **ninguna** (8N1) |
| Bits de parada | **1** |
| Timeout RX (`HAL_UART_Receive`) | **10 ms** (`UART_DATA_MIN_RX_TIMEOUT`) |
| Tamano max. util para send (proteccion API) | **256** (`UART_DATA_MAX_SIZE`) |

---

### API UART de datos (`API_uart_data.h` / `.c`) _TODO Actualizar

- `bool_t uartInit(void)` Configura **USART3** (baudios, 8N1, FIFO deshabilitado tras ajustes) e inicializa el periferico. Devuelve **false** si falla `HAL_UART_Init` o la configuracion extendida.
- `void uartSendString(uint8_t *pstring)` Envia una cadena **terminada en `\0`**. No envia si puntero nulo, longitud 0 o `strlen >= UART_DATA_MAX_SIZE`.
- `void uartSendStringSize(uint8_t *pstring, uint16_t size)` Envia **exactamente** `size` bytes (eco local, datos binarios cortos, etc.).
- `bool_t uartReceiveStringSize(uint8_t *pstring, uint16_t size)` Recibe **size** bytes con timeout; **true** si llegaron todos a tiempo, **false** si timeout o error HAL.
- `void API_uart_data_test(void)` Mensaje de prueba al arranque (`"Iniciado"` por TX).

Handler global: **`UART_HandleTypeDef huart3`** (definido en `API_uart_data.c`). GPIO y reloj: **`bsp_uart3.c`** / **`bsp_uart3.h`**.

---

### Hardware y pinout (I2C1)

| Parametro | Valor |
|-----------|--------|
| Periferico | **I2C1** (`hi2c1`, `I2C1`) |
| Placa de referencia (BSP) | **NUCLEO-L4R5ZI** |
| **SCL** | **PB8** (AF4 I2C1) |
| **SDA** | **PB9** (AF4 I2C1) |
| Puerto GPIO | **GPIOB** |
| Modo lineas (BSP) | **Open-drain** (`GPIO_MODE_AF_OD`), pull **NOPULL**, velocidad **HIGH** |
| Reloj periferico (RCC) | **PCLK1** (`RCC_I2C1CLKSOURCE_PCLK1`) — ver `APB1` en `System/Inc/clock_config.h` |

---

### Parametros de bus (I2C1 en firmware)

| Parametro | Valor en firmware |
|-----------|-------------------|
| Rol | **Master** (Transmit / Receive) |
| Direccionamiento | **7 bits** (`I2C_ADDRESSINGMODE_7BIT`) |
| Registro **TIMING** (frecuencia SCL) | **0x00000F3B** (`hi2c1.Init.Timing`) — fijada respecto de `PCLK1` |
| Filtro analogico I2C | **habilitado** (`I2C_ANALOGFILTER_ENABLE`) |
| Filtro digital I2C | **0** (desactivado en la cuenta del HAL) |
| Dual address / General call | deshabilitados |
| **Esclavo de referencia (NFC)** | **PN532**; direccion 7 bits **0x24**; hacia el HAL: **`PN532_I2C_ADDR_SHIFT`** (`0x24 << 1`, ver `API_pn532.h`) |
| Timeouts I2C (alta capa) | p. ej. **500 ms** default PN532 (`PN532_CONFIG_DEFAULT_TIMEOUT`) — parametro `timeout` en `I2C1_Write` / `I2C1_Read` |

---

### API I2C1 (`API_i2c_1.h` / `.c`)

- `void I2C1_Init(void)` Inicializa **I2C1** (`Instance`, `Timing`, 7 bit, filtros analógico/digital segun bloque superior). En fallo de `HAL_I2C_Init` o `HAL_I2CEx_Config*` llama a **`Error_Handler()`**.
- `I2C_HandleTypeDef* I2C1_Get_Handler(void)` Devuelve el puntero al handle interno (uso p. ej. integracion HAL, depuracion).
- `HAL_StatusTypeDef I2C1_Write(uint16_t addr, uint8_t* pData, uint16_t size, uint32_t timeout)` **Master transmit**; `addr` en formato HAL (7 bits desplazados un bit, alineado con `PN532_I2C_ADDR_SHIFT`).
- `HAL_StatusTypeDef I2C1_Read(uint16_t addr, uint8_t* pData, uint16_t size, uint32_t timeout)` **Master receive**; mismas convenciones de `addr` y `timeout`.

Handler interno: **`I2C_HandleTypeDef hi2c1`** (`static` en `API_i2c_1.c`). GPIO, RCC y `HAL_I2C_MspInit` / `HAL_I2C_MspDeInit`: **`bsp_i2c1.c`** / **`bsp_i2c1.h`**.

Consumidor principal en este TP: **`API_pn532.c`** (comandos y lecturas hacia el **PN532**).

---

### LEDs usados (salida de acceso / `API_accesos_output.c`)

El firmware **no** enciende en la logica de acceso los LED **LD2** / **LD3** (`BSP_LED2` / `BSP_LED3`); solo usa los tres mapeados como **rojo / amarillo / verde** en `bsp_leds.h`.

| Uso (API) | Simbolo BSP | Pin (GPIOB) | Nota |
|-----------|-------------|------------|------|
| Rojo | `BSP_LED_ROJO` = `BSP_LED4` | **PB10** | p. ej. no autorizado / error |
| Amarillo | `BSP_LED_AMARILLO` = `BSP_LED5` | **PB11** | p. ej. pendiente / advertencia |
| Verde | `BSP_LED_VERDE` = `BSP_LED6` | **PB0** | p. ej. acceso OK |

Inicializacion y drivers: **`bsp_leds.c`** / **`bsp_leds.h`**. Patrones por estado: **`API_accesos_output.c`**.

---

## Arquitectura en capas

| Capa | Carpeta principal | Rol |
|------|-------------------|-----|
| **BSP** | `BSP/Inc`, `BSP/Src` | Placa: pines, `HAL_*_MspInit`, LEDs, boton, mapeo **USART3** / **I2C1**. Depende de **HAL** y **CMSIS**. |
| **Drivers** | `Drivers/inc`, `Drivers/src` | Acceso de bajo nivel al bus: **I2C1** (`API_i2c_1`), **UART** de datos (`API_uart_data`). Usa **BSP** + HAL. |
| **API** | `API/inc`, `API/src` | Servicios de periférico y protocolo: **PN532** (`API_pn532`), salidas de acceso, **delay**, **debounce**, etc. Usa **Drivers** + **BSP** + HAL. |
| **APP** | `APP/inc`, `APP/src` | Aplicación del TP: **MEF** de acceso (`APP_acceso_fsm`), **parser** de comandos UART del autorizador (`APP_cmd_data_parser`). Usa **API** y **Drivers** según el caso. |

**Dependencia (de arriba hacia abajo):** `APP` → `API` → `Drivers` → `BSP` → **HAL**. `Core` (*main*, interrupciones) orquesta el ciclo y llama a **APP** / **API** / **Drivers** / **BSP**.

---

## Drivers implementados

### Driver I2C (`Drivers/inc/API_i2c_1.h`, `Drivers/src/API_i2c_1.c`)

Este módulo encapsula el uso del **I2C1** como maestro usando la HAL de STM32. Mantiene un `I2C_HandleTypeDef` estático para la instancia I2C1 y expone un *getter* para devolver el puntero al *handler* si fuera necesario.

| Parámetro | Valor |
|-----------|--------|
| Modo | Maestro I2C |
| Periférico | I2C1 |
| Velocidad de bus | aprox. 100 kHz (modo Standard) |
| Formato de dirección | 7 bits |
| Mapeo físico | PB8 SCL, PB9 SDA |
| Tipo de uso | Polled driver |

**API expuesta:** `I2C1_Init`, `I2C1_Write`, `I2C1_Read`, `I2C1_Get_Handler`

**NOTA:** La descripción de cada función está en el `.h` del archivo correspondiente en el repositorio.

---

### Driver UART (`Drivers/inc/API_uart_data.h`, `Drivers/src/API_uart_data.c`)

Este módulo encapsula el uso de la **USART3** como interfaz serie mediante la HAL de STM32. Mantiene un `UART_HandleTypeDef` estático para la instancia USART3 (no expone *getter* del *handler*, a diferencia del I2C).

| Parámetro | Valor |
|-----------|--------|
| Modo | Transmisión y recepción por `HAL_UART_Transmit` / `HAL_UART_Receive` (operación bloqueante con *timeout*) |
| Periférico | USART3 |
| Velocidad de línea | 57600 baud (8N1: 8 bits, sin paridad, 1 bit de parada; *oversampling* 16, sin control de flujo por hardware) |
| Formato / límites lógicos | Tramas acotadas por `UART_DATA_MAX_SIZE` (256); transmisión por cadena con `strlen` (cadena terminada en `\0`); recepción por tamaño con *timeout* mínimo `UART_DATA_MIN_RX_TIMEOUT` (10 ms) |
| Mapeo físico | PD8 TX, PD9 RX, función alternativa AF7 (BSP: VCP / ST-LINK en NUCLEO-L4R5ZI) |
| Tipo de uso | Polled driver |

**API expuesta:** `API_uart_data_init`, `uartSendString`, `uartSendStringSize`, `uartReceiveStringSize`, `uartSendCardToAuthorize`, `API_uart_data_test`

**NOTA:** La descripción de cada función está en el `.h` del archivo correspondiente en el repositorio.

---

### API PN532 (`API/inc/API_pn532.h`, `API/src/API_pn532.c`)

Capa de servicio de dispositivo sobre el protocolo **PN532** usando el **I2C1** del proyecto (`I2C1_Write` / `I2C1_Read`). Mantiene un manejador interno (`PN532_Handler_t`) con dirección I2C, *timeout* de operación, presencia de tarjeta y UID leído.

| Parámetro | Valor |
|-----------|--------|
| Bus / dirección | I2C, esclavo 7 bits `0x24` (módulos típicos); dirección en formato HAL desplazada (`PN532_I2C_ADDR_SHIFT`) |
| Modo de uso previsto | InListPassiveTarget (lectura bajo demanda); InAutoPoll definido a nivel de API pero el desarrollo apunta al modo pasivo |
| Tipo de uso | Acoplado al driver I2C bloqueante (*polled*); errores como `PN532_Status_t` (OK, sin tarjeta, I2C, *timeout*, protocolo, etc.) |

**API expuesta:** `PN532_init_module`, `PN532_config_module`, `PN532_polling_send`, `PN532_read_inlist_response`, `PN532_read_ack`, `PN532_save_read_uid_hex`

**NOTA:** Detalle de cada función en el `.h` del repositorio.

---

## Máquina de estados principal (`APP/inc/APP_acceso_fsm.h`, `APP/src/APP_acceso_fsm.c`)

La **MEF** de acceso es la aplicación cíclica: lectura NFC por **PN532** (I2C), envío del identificador al **autorizador** por **UART**, espera de la decisión remota y acción sobre **salidas** (LEDs / accesos). Usa un **`delay_t`** no bloqueante para temporizar pasos; en el estado de error se usa **`HAL_Delay`** dentro de un bucle (bloqueante durante la secuencia).

| Parámetro (macro) | Valor | Uso en la MEF |
|-------------------|-------|----------------|
| `ACCESO_DELAY_DEFAULT` | **500** ms | Espaciado general entre ciclos, esperas en *Init* / *Config* / *Idle* / *Poll* / *Wait RX* |
| `ACCESO_DELAY_TINY_DEFAULT` | **200** ms | Reintento corto tras fallo de **ACK** en sondeo |
| `ACCESO_DELAY_EXECUTE` | **3000** ms | Tiempo en que se mantienen indicadores tras decisión de acceso |
| `ACCESO_MAX_RETRIES` | **2** | Reintentos de ACK NFC y de espera de respuesta del autorizador |

**Entrada externa a la MEF:** `acceso_push_authorization(accesoExec_t)` actualiza el resultado del autorizador cuando `pending_authorization` está activo (solo si `exe_authorization` sigue en `ACCESO_EXEC_PENDIENTE`).

### Estados (`accesoState_t`) y comportamiento

| Estado | Descripción resumida |
|--------|------------------------|
| **`ACCESO_FSM_INIT`** | Arranque: limpieza de contexto, `PN532_init_module` (modo *InListPassiveTarget*). Tras **500 ms** y apagado de salidas, pasa a **Config**. Fallo NFC → **Error LED**. |
| **`ACCESO_FSM_CONFIG_PN532`** | `PN532_config_module`; tras temporización, `PN532_read_ack`. ACK OK → **Idle** con *delay* 500 ms; fallo → **Error LED**. |
| **`ACCESO_FSM_IDLE`** | Espera **500 ms** (*dead time* entre lecturas) y pasa a **Poll to read card**. |
| **`ACCESO_FSM_POLL_TO_READ_CARD`** | `PN532_polling_send`. Si falla → vuelve a **Idle**; si OK → **Poll read ACK** con *delay* 500 ms. |
| **`ACCESO_FSM_POLL_READ_ACK`** | Espera fin de *delay* **o** pulsación del **botón** (`BSP_Button_Read`). Luego `PN532_read_ack`. OK → **Process and validate**; si no, reintentos con *delay* **200 ms** hasta `ACCESO_MAX_RETRIES`, luego **Idle**. |
| **`ACCESO_FSM_PROCESS_AND_VALIDATE_DATA`** | Espera *delay* **o** botón en estado bajo (`BSP_Button_Read() == 0`). `PN532_read_inlist_response`; si OK, `PN532_save_read_uid_hex` en `value_card`. Con UID válido → **Sent to authorizer**; sin tarjeta (`PN532_ERR_NO_CARD`) u otros casos → **Idle** (y limpieza de buffers según rama). |
| **`ACCESO_FSM_SENT_TO_AUTHORIZER`** | Si no hay tarjeta válida → **Error LED**. Si no: marca espera de autorización, `uartSendCardToAuthorize`, *delay* 500 ms y estado **Wait RX**. |
| **`ACCESO_FSM_WAIT_RX`** | Si llegó decisión por `acceso_push_authorization` (`exe_authorization != ACCESO_EXEC_PENDIENTE`) → **Exec acceso**. Si vence el *timeout* sin respuesta, incrementa reintentos: hasta `ACCESO_MAX_RETRIES` vuelve a **Sent**; si no, **Error LED**. |
| **`ACCESO_FSM_EXEC_ACCESO`** | Según `exe_authorization`: **acceso A**, **acceso B**, **no autorizado** o rama de error. Tras **3000 ms** (`ACCESO_DELAY_EXECUTE`) vuelve a **Init** (nuevo ciclo). |
| **`ACCESO_FSM_EXEC_ERROR_LED_SEQ`** | Secuencia fija de llamadas a `acceso_error_indicador()` con **50 ms** de `HAL_Delay` (bucle **16** iteraciones). Luego limpia pendientes y vuelve a **Idle**. |

**Resultado de autorización (`accesoExec_t`):** `ACCESO_EXEC_PENDIENTE`, `ACCESO_EXEC_NO_AUTORIZADO`, `ACCESO_EXEC_ACCESO_A`, `ACCESO_EXEC_ACCESO_B`, `ACCESO_EXEC_ERROR` (según definición en el `.h` y uso en `acceso_FSM_update`).

---

## Estructura del repositorio (`tree -L 4`)

Vista de carpetas de aplicación (sin `Drivers/CMSIS` ni `Drivers/STM32L4xx_HAL_Driver` por brevedad). Para regenerar en terminal: `tree -L 4 -I 'STM32L4xx_HAL_Driver|CMSIS'`.

```text
.
├── API
│   ├── inc
│   │   ├── API_accesos_output.h
│   │   ├── API_debounce.h
│   │   ├── API_delay.h
│   │   └── API_pn532.h
│   └── src
│       ├── API_accesos_output.c
│       ├── API_delay.c
│       └── API_pn532.c
├── APP
│   ├── inc
│   │   ├── APP_acceso_fsm.h
│   │   └── APP_cmd_data_parser.h
│   └── src
│       ├── APP_acceso_fsm.c
│       └── APP_cmd_data_parser.c
├── BSP
│   ├── Inc
│   │   ├── bsp.h
│   │   ├── bsp_button.h
│   │   ├── bsp_i2c1.h
│   │   ├── bsp_leds.h
│   │   └── bsp_uart3.h
│   └── Src
│       ├── bsp.c
│       ├── bsp_button.c
│       ├── bsp_i2c1.c
│       ├── bsp_leds.c
│       └── bsp_uart3.c
├── Core
│   ├── Inc
│   │   ├── main.h
│   │   ├── stm32l4xx_hal_conf.h
│   │   └── stm32l4xx_it.h
│   └── Src
│       ├── main.c
│       ├── stm32l4xx_hal_msp.c
│       ├── stm32l4xx_it.c
│       ├── syscalls.c
│       └── sysmem.c
└── Drivers
    ├── inc
    │   ├── API_i2c_1.h
    │   └── API_uart_data.h
    └── src
        ├── API_i2c_1.c
        └── API_uart_data.c
```
