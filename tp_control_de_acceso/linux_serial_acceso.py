import serial
import time


def resolver_acceso(lectura):
    if lectura == "CARDAC8BF81E":
        return "ACCESOA"
    if lectura == "CARD07996A62":
        return "ACCESOB"
    #if lectura == "CARDAC75061F"
    return "ACCESO0"


def main():
    puerto = "/dev/ttyUSB0"
    baudios = 57600

    try:
        ser = serial.Serial(port=puerto, baudrate=baudios, timeout=0.05)
        print(f"[{time.strftime('%H:%M:%S')}] Conectado al puerto {puerto} con {baudios} baudios.")
    except serial.SerialException as e:
        print(f"Error al conectar al puerto: {e}")
        return

    rx_buffer = bytearray()
    ultimo_rx = None
    timeout_trama_s = 1.0

    try:
        while True:
            disponibles = ser.in_waiting
            if not disponibles:
                if rx_buffer and ultimo_rx and (time.monotonic() - ultimo_rx) > timeout_trama_s:
                    print("Trama incompleta descartada por timeout.")
                    rx_buffer.clear()
                    ultimo_rx = None
                continue

            datos = ser.read(disponibles)
            if not datos:
                continue

            rx_buffer.extend(datos)
            ultimo_rx = time.monotonic()

            while b"\n" in rx_buffer:
                trama_bytes, _, resto = rx_buffer.partition(b"\n")
                rx_buffer = bytearray(resto)

                lectura = trama_bytes.decode(errors="ignore").strip()
                if not lectura:
                    continue

                respuesta = resolver_acceso(lectura)
                ser.write(respuesta.encode())
                print(f"RX: {lectura} -> TX: {respuesta}")
    except KeyboardInterrupt:
        print("\nFinalizando el programa...")
    except Exception as e:
        print(f"Error en lectura serial: {e}")
    finally:
        ser.close()
        print("Puerto serial cerrado.")


if __name__ == "__main__":
    main()
