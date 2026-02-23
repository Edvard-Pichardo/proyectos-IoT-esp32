# Proyecto IoT 1: Control Bluetooth con ESP32 y Sensor DHT11

![ESP32](https://img.shields.io/badge/ESP32-Desarrollo-blue) ![Bluetooth](https://img.shields.io/badge/Bluetooth-SPP-green) ![DHT11](https://img.shields.io/badge/Sensor-DHT11-orange)

## Descripción General 

En esta práctica se implementa un sistema de control y monitoreo utilizando un **ESP32** que se comunica vía **Bluetooth** con un dispositivo externo (como un smartphone o computadora). 

El sistema permite:

- Encender y apagar 5 LEDs de forma individual o todos a la vez mediante comandos enviados por Bluetooth.
- Leer la temperatura y la humedad desde un sensor **DHT11** al presionar dos pulsadores físicos conectados al ESP32, y enviar los datos al dispositivo conectado por Bluetooth.

El código está desarrollado en el entorno de Arduino (compatible con PlatformIO) y hace uso de las librerías `BluetoothSerial` y `DHT` para manejar la comunicación y el sensor.

## Componentes Necesarios

| Componente               | Cantidad | Notas                                           |
|--------------------------|----------|-------------------------------------------------|
| ESP32 (cualquier modelo) | 1        | Se usa el puerto serie y Bluetooth              |
| Sensor DHT11             | 1        | Para medir temperatura y humedad                |
| LEDs (colores variados)  | 5        | Pueden ser de 5mm o 3mm                         |
| Resistencias de 220Ω     | 5        | Para limitar corriente en los LEDs              |
| Pulsadores (push buttons)| 2        | Uno con pull-down, otro con pull-up interno     |
| Resistor 10kΩ (para SW1) | 1        | Solo necesario si se usa pull-down externo      |
| Protoboard y cables      | -        | Para realizar las conexiones                     |

## Diagrama de Conexiones

A continuación se describen las conexiones físicas entre los componentes y el ESP32:

| Componente      | Pin del ESP32 | Notas                                                          |
|-----------------|---------------|----------------------------------------------------------------|
| LED1            | GPIO 14       | Ánodo al pin, cátodo a GND (con resistencia de 220Ω)          |
| LED2            | GPIO 27       | Igual que LED1                                                 |
| LED3            | GPIO 26       | Igual que LED1                                                 |
| LED4            | GPIO 25       | Igual que LED1                                                 |
| LED5            | GPIO 33       | Igual que LED1                                                 |
| Pulsador SW1    | GPIO 15       | Conectar entre el pin y VCC (3.3V) con resistencia pull-down de 10kΩ a GND |
| Pulsador SW2    | GPIO 4        | Conectar entre el pin y GND (usando resistencia pull-up interna del ESP32) |
| Sensor DHT11    | GPIO 32       | VCC a 3.3V, GND a GND, Data al pin 32                          |

**Nota:** Para SW2 se utiliza la resistencia pull-up interna del ESP32, por lo que no se requiere resistor externo; el pin se conecta directamente a GND cuando se presiona.

## Configuración del Entorno

### Arduino IDE

1. Instala el soporte para ESP32 en el Arduino IDE siguiendo la [guía oficial](https://github.com/espressif/arduino-esp32).
2. Instala las librerías necesarias:
   - **BluetoothSerial** (viene incluida con el paquete de ESP32).
   - **DHT sensor library** de Adafruit (búscala en el Gestor de Librerías).
3. Selecciona la placa **ESP32 Dev Module** y el puerto COM correspondiente.
4. Copia el código proporcionado y súbelo a tu ESP32.

### PlatformIO

Si usas PlatformIO, crea un nuevo proyecto para la placa `espressif32` y añade las siguientes dependencias en el archivo `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    adafruit/DHT sensor library@^1.4.4
monitor_speed = 115200
