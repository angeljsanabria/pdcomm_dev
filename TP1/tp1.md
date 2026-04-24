# Protocolo I2C/SPI

## Actividades a realizar

1. Seleccione un circuito integrado de interés, el cual puede ser cualquier tipo de sensor, expansor de IO, memoria EEPROM, controlador de LCD, entre otros. Una vez seleccionado, descargue su hoja de datos e incluya la primera página en este trabajo práctico (una captura de imagen es aceptable). De una breve explicación de las funcionalidades del CI seleccionado.

   **NOTA:** Se propone que en el caso de que el estudiante ya tenga definido su trabajo final de carrera y el mismo incluye un CI que se comunica por I2C o SPI, esta actividad sea basada en ese dispositivo.

2. Dentro de la hoja de datos, identifique un diagrama de tiempos de la comunicación (usualmente se muestra bajo la leyenda *bus timing*) y construya una tabla identificando:

   | Parámetro | Valor (según hoja de datos) |
   |-----------|-----------------------------|
   | a) Frecuencia máxima de operación | |
   | b) Tiempo mínimo de pulso de clock en alto | |
   | c) Tiempo mínimo de pulso de clock en bajo | |

### Si eligió I2C

Complete la tabla con:

| Parámetro | Valor (según hoja de datos) |
|-----------|-----------------------------|
| d) Tiempo máximo de flanco ascendente (rise time) | |
| e) Tiempo máximo de flanco descendente (fall time) | |

**h)** ¿Puede determinar una relación entre el punto a) y los puntos b) a e)?

### Si eligió SPI

Complete la tabla con:

| Parámetro | Valor (según hoja de datos) |
|-----------|-----------------------------|
| d) Tiempo mínimo de seteo para CS (setup time - inicio de trama) | |
| e) Tiempo mínimo de mantenimiento para CS (hold time - fin de trama) | |
| f) Tiempo mínimo para CS válido (tiempo mínimo entre tramas) | |
| g) Tiempo máximo de mantenimiento de datos (input hold time) | |

**h)** ¿Puede determinar una relación entre el punto a) y los puntos b) a g)?

---

## 3. Diagramas lógicos I2C (escritura y lectura)

A continuación se muestran secuencias típicas en **modo maestro** hacia un esclavo con **mapa de registros** (caso del ADXL345 y sensores similares). Cada celda es un **byte** enviado en serie por SDA; el maestro genera **SCL**. **A** = bit de **ACK** (confirmación del receptor); **N** = **NACK** (último byte en lectura, asume el maestro).

### 3.1 Escritura en un registro (write)

Orden lógico de la trama:

| Fase | Contenido en el bus | Significado |
|------|---------------------|-------------|
| Inicio | **S** (START) | El maestro toma el bus; condición de inicio. |
| Byte 1 | **ADDR[6:0]** + **W=0** | **Dirección física de 7 bits** del esclavo en el bus I2C; el bit menos significativo es **R/W** (0 = escritura). |
| ACK | **A** | El esclavo reconoce su dirección. |
| Byte 2 | **REG[7:0]** | **Dirección del registro** interno del dispositivo (qué registro se escribe). |
| ACK | **A** | El esclavo acepta la subdirección. |
| Byte 3… | **DATA[7:0]** (uno o más) | **Datos** a grabar en el registro (o secuencia si el dispositivo autoincrementa). |
| ACK | **A** (por byte) | Confirmación por cada byte de datos. |
| Fin | **P** (STOP) | Libera el bus. |

Diagrama lógico (una palabra de datos):

```text
     S    [ ADDR | W ]    A    [ REG ]    A    [ DATA ]    A    P
          <-- 8 bits -->       8 bits         8 bits
```

- **ADDR[6:0]**: dirección I2C de 7 bits del chip (p. ej. configuración del pin `ALT ADDRESS` en el ADXL345).
- **W**: bit de lectura/escritura en el primer byte de dirección (0 = escritura).
- **REG**: “dirección lógica” / puntero al registro interno.
- **DATA**: carga útil.

### 3.2 Lectura de un registro (read)

Para leer, el maestro primero **escribe** el registro a leer y luego **cambia a lectura** (lectura con **repeated START**):

| Fase | Contenido | Significado |
|------|-----------|-------------|
| **S** | START | Inicio. |
| **[ ADDR \| W=0 ]** + **A** | Dirección 7 bits + escritura | El maestro indica que enviará la subdirección. |
| **[ REG ]** + **A** | Dirección de registro | Qué registro se va a leer. |
| **Sr** | **repeated START** | Nueva condición de inicio sin STOP intermedio. |
| **[ ADDR \| R=1 ]** + **A** | Dirección 7 bits + lectura | El esclavo pasa a enviar datos. |
| **[ DATA ]** + **N** | Dato(s) | El esclavo envía bytes; el maestro hace **ACK** en bytes intermedios y **NACK** en el último. |
| **P** | STOP | Fin. |

Diagrama lógico (un byte de lectura):

```text
     S   [ ADDR|W ] A [ REG ] A  Sr  [ ADDR|R ] A [ DATA ] N  P
```

- **R=1**: mismo campo de 8 bits que antes, pero con el bit R/W en **1** (lectura).
- **Sr**: *repeated START*; separa la fase “apunto el registro” de la fase “leo el dato”.

---

## 4. HAL de STM32 (I2C): funciones de transmisión y recepción

**Documento de referencia:** *Description of STM32F4xx HAL and low-layer drivers* (UM1720 / DM00105879), sección del driver **I2C** — grupo de funciones de comunicación en `stm32f4xx_hal_i2c.c` / `stm32f4xx_hal_i2c.h`.

### 4.1 Funciones identificadas (maestro, polling)

| Operación | Función HAL | Rol |
|-----------|-------------|-----|
| Transmisión | `HAL_I2C_Master_Transmit` | El maestro envía un bloque de bytes al esclavo (después de generar START y la secuencia de dirección interna de la HAL). |
| Recepción | `HAL_I2C_Master_Receive` | El maestro lee un bloque de bytes del esclavo. |

**Prototipos (extraídos del estilo HAL STM32F4):**

```c
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
                                         uint16_t DevAddress,
                                         uint8_t *pData,
                                         uint16_t Size,
                                         uint32_t Timeout);

HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c,
                                         uint16_t DevAddress,
                                         uint8_t *pData,
                                         uint16_t Size,
                                         uint32_t Timeout);
```

**Parámetros (resumen):**

- **`hi2c`**: manejador del periférico I2C ya inicializado (`MX_I2Cx_Init`).
- **`DevAddress`**: dirección del esclavo en el formato que exige la HAL (habitualmente la **dirección de 7 bits desplazada un lugar a la izquierda**, p. ej. `0x1D << 1` si el 7-bit address es `0x1D`).
- **`pData`**: puntero al buffer con datos a enviar o donde guardar los leídos.
- **`Size`**: cantidad de bytes.
- **`Timeout`**: tiempo máximo de espera en milisegundos.

**Funcionamiento breve:** la HAL configura el periférico, genera las condiciones **START/STOP**, arma el primer byte con **dirección + R/W** según la función, y transfiere los bytes en **polling** (en esta variante) hasta completar `Size` o fallar por timeout/error de bus. Devuelve `HAL_OK` o códigos de error (`HAL_BUSY`, `HAL_ERROR`, etc.).

### 4.2 Nota para dispositivos con registro interno (p. ej. ADXL345)

Si el protocolo del chip exige primero enviar la **dirección de registro** y luego leer/escribir datos, suele usarse el par **memoria/registro** de la HAL (misma familia de drivers):

- `HAL_I2C_Mem_Write` — escribe en una “posición” interna (registro + datos).
- `HAL_I2C_Mem_Read` — lee desde una posición interna.

Encapsulan la secuencia **START – ADDR – subdirección – datos – STOP** (y **repeated START** en lectura) sin armar manualmente cada byte en muchos casos.

**Referencia:** [DM00105879 — Description of STM32F4xx HAL and low-layer drivers](https://www.st.com/resource/en/user_manual/dm00105879-description-of-stm32f4-hal-and-lowlayer-drivers-stmicroelectronics.pdf).
