# Proyecto IoT 3: Comunicación MQTT con ESP32 – Publicación y Suscripción de Datos

![ESP32](https://img.shields.io/badge/Plataforma-ESP32-blue) ![MQTT](https://img.shields.io/badge/Protocolo-MQTT-brightgreen) ![DHT11](https://img.shields.io/badge/Sensor-DHT11-orange) ![Broker](https://img.shields.io/badge/Broker-mosquitto-lightgrey)

## Descripción General

Este es el tercer proyecto de la serie IoT, en el cual se implementa un **cliente MQTT** en el ESP32 para comunicarse con un broker (en este caso, `test.mosquitto.org`). El sistema publica lecturas de sensores (fotoresistencia, potenciómetro y DHT11) y se suscribe a un tópico para recibir comandos que controlan cinco LEDs.

**Funcionalidades principales:**

- **Publicación manual:** Al presionar dos botones físicos (pull‑up y pull‑down), se publican los valores de la fotoresistencia y el potenciómetro en tópicos específicos.
- **Publicación automática:** Cada 5 segundos, el sensor DHT11 envía la temperatura y humedad a sus respectivos tópicos.
- **Suscripción y control:** El ESP32 está suscrito al tópico `ClaseIoT/Pichardo/Led`; al recibir un número (0‑9), ejecuta acciones sobre los LEDs:
  - `0` → apaga todos.
  - `1` a `5` → enciende el LED correspondiente.
  - `6` → enciende LEDs impares (1, 3, 5).
  - `7` → enciende LEDs pares (2, 4).
  - `8` → efecto "tren" ascendente.
  - `9` → efecto "tren" descendente.

Este proyecto muestra el uso del protocolo **MQTT**, ligero y eficiente, ideal para entornos IoT donde se requiere comunicación entre múltiples dispositivos y aplicaciones (dashboards, bases de datos, etc.).

## Componentes Necesarios

| Componente               | Cantidad | Notas                                           |
|--------------------------|----------|-------------------------------------------------|
| ESP32 (cualquier modelo) | 1        | Cliente MQTT                                    |
| Sensor DHT11             | 1        | Temperatura y humedad                           |
| Fotoresistencia (LDR)    | 1        | Por ejemplo, GL5528                             |
| Potenciómetro            | 1        | 10kΩ                                            |
| LEDs (colores variados)  | 5        | Con resistencias de 220Ω                       |
| Resistencias de 220Ω     | 5        | Para los LEDs                                   |
| Resistor de 10kΩ         | 1        | Para la fotoresistencia (divisor de tensión)    |
| Botones (pulsadores)     | 2        | Uno con pull‑up interno y otro con pull‑down externo |
| Protoboard y cables      | -        | Para las conexiones                             |

## Diagrama de Conexiones

| Componente          | Pin del ESP32 | Notas                                                          |
|---------------------|---------------|----------------------------------------------------------------|
| LED1                | GPIO 14       | Ánodo al pin, cátodo a GND (con resistencia de 220Ω)          |
| LED2                | GPIO 27       | Igual que LED1                                                 |
| LED3                | GPIO 26       | Igual que LED1                                                 |
| LED4                | GPIO 25       | Igual que LED1                                                 |
| LED5                | GPIO 33       | Igual que LED1                                                 |
| Botón Pull‑up       | GPIO 4        | Conectar entre el pin y GND (activo bajo)                      |
| Botón Pull‑down     | GPIO 15       | Conectar entre el pin y VCC (3.3V) con resistencia pull‑down de 10kΩ a GND (activo alto) |
| Fotoresistencia (LDR)| GPIO 34       | En serie con una resistencia de 10kΩ a GND; punto medio al pin; otro extremo a 3.3V |
| Potenciómetro       | GPIO 35       | Pin central al ADC, extremos a 3.3V y GND                      |
| DHT11               | GPIO 32       | VCC a 3.3V, GND a GND, Data al pin 32                          |

**Nota:** El botón pull‑up utiliza la resistencia interna del ESP32 (configurada con `INPUT_PULLUP`), por lo que no requiere componentes externos.

## Configuración del Entorno

### Arduino IDE

1. Instala el soporte para ESP32 en el Arduino IDE (guía oficial).
2. Instala las siguientes librerías desde el Gestor de Librerías:
   - **EspMQTTClient** (de Patrick Lapointe) – cliente MQTT fácil de usar.
   - **DHT sensor library** de Adafruit.
3. Selecciona la placa **ESP32 Dev Module** y el puerto COM correspondiente.
4. Ajusta las credenciales WiFi y el broker MQTT (si deseas otro broker) en las líneas correspondientes del código.
5. Carga el programa al ESP32.

## Explicación del Código

### Bibliotecas utilizadas:

- EspMQTTClient: Simplifica la conexión WiFi y MQTT, gestionando reconexiones automáticas.

- DHT: Lectura del sensor DHT11.

### Configuración inicial:

- Se definen pines, tópicos y credenciales de red.

- Se crea el objeto EspMQTTClient con los parámetros WiFi y broker.

Función onConnectionEstablished():
Se ejecuta automáticamente cuando el cliente se conecta al broker. Aquí se suscribe al tópico de control de LEDs y se asocia un callback (handleLedControl) para procesar los mensajes entrantes.

- Función `handleLedControl(payload)`:
Recibe el payload (convertido a entero) y ejecuta las acciones correspondientes según el valor (0‑9). Incluye efectos de iluminación como encender pares/impares y secuencias tipo "tren".

- Bucle `loop()`:

`client.loop()`: Mantiene la conexión MQTT activa y procesa mensajes entrantes.

`Botón pull‑up` (activo bajo): Publica el valor de la fotoresistencia cuando se presiona (con antirrebote).

`Botón pull‑down` (activo alto): Publica el valor del potenciómetro.

`Temporizador DHT`: Cada 5 segundos, lee el sensor y publica temperatura y humedad en sus tópicos.
