# Proyecto IoT 4: Control y Monitoreo con Telegram Bot

![ESP32](https://img.shields.io/badge/Plataforma-ESP32-blue) ![Telegram Bot](https://img.shields.io/badge/API-Telegram%20Bot-26A5E4) ![WiFi](https://img.shields.io/badge/Comunicación-WiFi-yellow) ![Alarma](https://img.shields.io/badge/Funcionalidad-Alarma-red)

## Descripción General

Este es el cuarto proyecto de la serie IoT, en el cual se implementa un **bot de Telegram** para controlar y monitorear dispositivos desde cualquier lugar mediante mensajes de texto. El ESP32 se conecta a WiFi y utiliza la API de Telegram para recibir comandos y enviar respuestas en tiempo real.

**Comandos disponibles:**
- `/led [1-5] on/off` → Enciende o apaga un LED específico.
- `/pot` → Devuelve el valor actual del potenciómetro (0‑4095).
- `/rangoAlarma [0-4095]` → Ajusta el umbral de la alarma de luz.
- `/alarma on/off` → Activa o desactiva el sistema de alarma.

**Funcionamiento de la alarma:**  
Cuando la alarma está activada, el bot monitorea continuamente el nivel de luz (fotoresistencia). Si el valor cae por debajo del umbral configurado, envía automáticamente un mensaje de alerta al usuario.

Este proyecto demuestra cómo integrar dispositivos IoT con servicios de mensajería populares, facilitando la interacción remota sin necesidad de aplicaciones móviles dedicadas.

## Componentes Necesarios

| Componente               | Cantidad | Notas                                           |
|--------------------------|----------|-------------------------------------------------|
| ESP32 (cualquier modelo) | 1        | Cliente WiFi y bot de Telegram                  |
| Fotoresistencia (LDR)    | 1        | Por ejemplo, GL5528                             |
| Potenciómetro            | 1        | 10kΩ                                            |
| LEDs (colores variados)  | 5        | Con resistencias de 220Ω                       |
| Resistencias de 220Ω     | 5        | Para los LEDs                                   |
| Resistor de 10kΩ         | 1        | Para la fotoresistencia (divisor de tensión)    |
| Protoboard y cables      | -        | Para las conexiones                             |

## Diagrama de Conexiones

| Componente          | Pin del ESP32 | Notas                                                          |
|---------------------|---------------|----------------------------------------------------------------|
| LED1                | GPIO 14       | Ánodo al pin, cátodo a GND (con resistencia de 220Ω)          |
| LED2                | GPIO 27       | Igual que LED1                                                 |
| LED3                | GPIO 26       | Igual que LED1                                                 |
| LED4                | GPIO 25       | Igual que LED1                                                 |
| LED5                | GPIO 33       | Igual que LED1                                                 |
| Fotoresistencia (LDR)| GPIO 34       | En serie con una resistencia de 10kΩ a GND; punto medio al pin; otro extremo a 3.3V |
| Potenciómetro       | GPIO 35       | Pin central al ADC, extremos a 3.3V y GND                      |

## Configuración del Entorno

### Prerrequisitos

1. **Crear un bot de Telegram:**
   - Abre Telegram y busca **@BotFather**.
   - Envía el comando `/newbot` y sigue las instrucciones para crear tu bot.
   - Al finalizar, recibirás un **token de acceso** (por ejemplo, `123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11`). Cópialo.

2. **Obtener tu `chat_id` (opcional, pero útil para pruebas):**
   - Envía un mensaje a tu bot.
   - Visita la URL `https://api.telegram.org/bot<TU_TOKEN>/getUpdates` (reemplaza `<TU_TOKEN>` con tu token).
   - Busca el campo `"chat":{"id": ...}`. Ese número es tu `chat_id`. No es estrictamente necesario en el código, pero puede ser útil para depuración.

### Arduino IDE

1. Instala el soporte para ESP32 en el Arduino IDE (guía oficial).
2. Instala las siguientes librerías desde el Gestor de Librerías:
   - **UniversalTelegramBot** (de Brian Lough) – maneja la API de Telegram.
   - **WiFi** (incluida con el core de ESP32).
3. Selecciona la placa **ESP32 Dev Module** y el puerto COM correspondiente.
4. Ajusta las credenciales WiFi y el token de tu bot en el código.
5. Carga el programa al ESP32.

## Explicación del Código

**Bibliotecas utilizadas:**
- `WiFi.h`: Conexión a red WiFi.
- `UniversalTelegramBot.h`: Cliente para la API de Telegram.
- `WiFiClientSecure.h`: Conexión HTTPS para comunicarse con Telegram de forma segura.

**Configuración inicial:**
- Se definen las credenciales WiFi y el token del bot.
- Se crean los objetos `WiFiClientSecure` y `UniversalTelegramBot`.
- En `setup()` se conecta a WiFi y se configura el cliente HTTPS en modo inseguro (`setInsecure()`) para evitar problemas con certificados (común en ESP32).

**Función `handleNewMessages(int numNewMessages)`:**
- Procesa cada nuevo mensaje recibido.
- Analiza el texto y ejecuta el comando correspondiente:
  - `/led`: Extrae el número del LED y la acción, y cambia el estado del LED. Responde con confirmación.
  - `/pot`: Lee el valor analógico del potenciómetro y lo envía como respuesta.
  - `/rangoAlarma`: Valida el valor ingresado (0-4095) y actualiza el umbral.
  - `/alarma on/off`: Activa o desactiva la alarma.

**Bucle `loop()`:**
- Cada 500 ms (`botRequestDelay`) consulta si hay nuevos mensajes mediante `bot.getUpdates()`.
- Si hay mensajes nuevos, llama a `handleNewMessages()`.
- Si la alarma está activada, lee continuamente el valor de la fotoresistencia. Si el nivel de luz es inferior al umbral, envía una notificación de alerta.

**Nota:** La alarma envía un mensaje cada vez que se cumple la condición en el `loop()`. Para evitar spam, sería recomendable añadir una bandera que evite enviar múltiples notificaciones consecutivas hasta que la condición cambie.

## Instrucciones de Uso

1. **Arma el circuito** según el diagrama de conexiones.
2. **Crea tu bot de Telegram** con BotFather y obtén el token.
3. **Configura las credenciales WiFi** y el token en el código.
4. **Carga el programa** al ESP32.
5. **Abre el monitor serie** (115200 baudios) para ver mensajes de depuración.
6. **Inicia una conversación con tu bot** en Telegram.
7. **Prueba los comandos:**
   - Enviar `/led 1 on` → Enciende LED1.
   - Enviar `/led 2 off` → Apaga LED2.
   - Enviar `/pot` → Recibirás el valor del potenciómetro.
   - Enviar `/rangoAlarma 700` → Establece el umbral en 700.
   - Enviar `/alarma on` → Activa la alarma.
   - Cubre la fotoresistencia para simular oscuridad y verás el mensaje de alerta.
8. **Observa las respuestas del bot** en tiempo real.

## Posibles Mejoras

- Añadir un sistema de autenticación para restringir el bot a usuarios autorizados (comparando `chat_id`).
- Implementar un debounce para evitar notificaciones repetitivas de la alarma.
- Agregar más comandos: control de servos, lectura de temperatura, etc.
- Utilizar el bot para enviar notificaciones periódicas (por ejemplo, cada hora) con el estado de los sensores.
- Integrar con servicios como IFTTT o Zapier para acciones adicionales.
- Almacenar la configuración (umbral, estado de la alarma) en EEPROM para que persista tras reinicios.

## Autor

Pichardo Rico Cristian Eduardo



