# Proyecto IoT 5: Control por Voz con SinricPro (Alexa / Google Home)

![ESP32](https://img.shields.io/badge/Plataforma-ESP32-blue) ![SinricPro](https://img.shields.io/badge/Plataforma-SinricPro-brightgreen) ![Alexa](https://img.shields.io/badge/Asistente-Alexa-00BFFF) ![Google Home](https://img.shields.io/badge/Asistente-Google%20Home-FFD700) ![Control por Voz](https://img.shields.io/badge/Funcionalidad-Control%20por%20Voz-red)

## Descripción General

Este es el quinto proyecto de la serie IoT, en el cual se implementa el control de dispositivos mediante **comandos de voz** utilizando **Alexa** o **Google Home** a través de la plataforma **SinricPro**. El ESP32 se conecta a WiFi y se registra en la nube de SinricPro, permitiendo que los asistentes de voz envíen órdenes de encendido/apagado a tres LEDs (o relés) conectados al microcontrolador.

**Características principales:**
- Control de tres dispositivos (LEDs/relés) mediante comandos de voz.
- Integración con Alexa y Google Home a través de SinricPro.
- Comunicación mediante WebSockets para respuesta en tiempo real.
- Configuración sencilla a través del portal de SinricPro.
- Callbacks para manejar los cambios de estado de cada dispositivo.

Este proyecto demuestra cómo integrar dispositivos IoT con los ecosistemas de domótica más populares, facilitando el control por voz sin necesidad de aplicaciones móviles adicionales.

## Componentes Necesarios

| Componente               | Cantidad | Notas                                           |
|--------------------------|----------|-------------------------------------------------|
| ESP32 (cualquier modelo) | 1        | Cliente WiFi y controlador                       |
| LEDs (colores variados)  | 3        | Con resistencias de 220Ω                        |
| Resistencias de 220Ω     | 3        | Para los LEDs                                   |
| Protoboard y cables      | -        | Para las conexiones                             |

**Opcional:** En lugar de LEDs, se pueden utilizar relés para controlar dispositivos de mayor potencia (lámparas, electrodomésticos, etc.).

## Diagrama de Conexiones

| Componente          | Pin del ESP32 | Notas                                                          |
|---------------------|---------------|----------------------------------------------------------------|
| LED1 (Switch 1)     | GPIO 14       | Ánodo al pin, cátodo a GND (con resistencia de 220Ω)          |
| LED2 (Switch 2)     | GPIO 27       | Igual que LED1                                                 |
| LED3 (Switch 3)     | GPIO 26       | Igual que LED1                                                 |

## Configuración del Entorno

### Prerrequisitos

1. **Crear una cuenta en SinricPro:**
   - Visita [SinricPro](https://sinricpro.com/) y regístrate.
   - Una vez dentro, ve al panel de control.

2. **Crear dispositivos (switches):**
   - En el panel de SinricPro, haz clic en "Add Device".
   - Selecciona "Switch" como tipo de dispositivo.
   - Asigna un nombre (ej. "LED 1", "LED 2", "LED 3").
   - Completa el proceso para cada uno de los tres switches.
   - Anota los **Device IDs** que aparecen al crear cada dispositivo.

3. **Obtener credenciales de la aplicación:**
   - En el panel de SinricPro, ve a "Dashboard" o "Credentials".
   - Copia el **APP_KEY** y **APP_SECRET**.

4. **Vincular con Alexa o Google Home:**
   - En la aplicación de Alexa o Google Home, busca la habilidad "SinricPro".
   - Inicia sesión con tus credenciales de SinricPro.
   - Los dispositivos creados aparecerán automáticamente y podrás controlarlos por voz.

### Arduino IDE

1. Instala el soporte para ESP32 en el Arduino IDE (guía oficial).
2. Instala las siguientes librerías desde el Gestor de Librerías:
   - **SinricPro** (de SinricPro) – maneja la conexión con la plataforma.
   - **WiFi** (incluida con el core de ESP32).
3. Selecciona la placa **ESP32 Dev Module** y el puerto COM correspondiente.
4. Ajusta las credenciales WiFi, APP_KEY, APP_SECRET y los Device IDs en el código.
5. Carga el programa al ESP32.

## Explicación del Código

**Bibliotecas utilizadas:**
- `WiFi.h`: Conexión a red WiFi (se adapta automáticamente a ESP8266, ESP32 o RP2040).
- `SinricPro.h`: Biblioteca principal para la comunicación con la plataforma SinricPro.
- `SinricProSwitch.h`: Para manejar dispositivos tipo switch (encendido/apagado).

**Configuración inicial:**
- Se definen las credenciales WiFi, APP_KEY, APP_SECRET y los Device IDs de los tres switches.
- Se configuran los pines de los LEDs como salida en `setupSinricPro()`.
- Cada switch se asocia a su Device ID y se registra un callback para manejar los cambios de estado.

**Funciones callback (`onPowerState1`, `onPowerState2`, `onPowerState3`):**
- Se ejecutan cuando SinricPro recibe un comando de encendido/apagado desde Alexa o Google Home.
- Reciben el `deviceId` (para identificar qué dispositivo) y el `state` (true/false).
- Cambian el estado del LED correspondiente y envían un mensaje por el monitor serie.
- Devuelven `true` para indicar que el comando fue procesado correctamente.

**Función `setupWiFi()`:**
- Conecta el ESP32 a la red WiFi configurada.
- Incluye configuraciones específicas para ESP8266 y ESP32 (desactivar modo de bajo consumo, auto-reconexión).

**Función `setupSinricPro()`:**
- Configura los pines de los LEDs como salida.
- Asocia cada Device ID con su callback correspondiente.
- Registra funciones para eventos de conexión/desconexión con SinricPro.
- Inicia la conexión con `SinricPro.begin(APP_KEY, APP_SECRET)`.

**Bucle `loop()`:**
- Llama continuamente a `SinricPro.handle()` para mantener la conexión activa y procesar los mensajes entrantes de la nube.

## Instrucciones de Uso

1. **Arma el circuito** según el diagrama de conexiones.
2. **Crea tu cuenta en SinricPro** y obtén APP_KEY, APP_SECRET y los Device IDs.
3. **Configura las credenciales WiFi** y los datos de SinricPro en el código.
4. **Carga el programa** al ESP32.
5. **Abre el monitor serie** (115200 baudios) para ver los mensajes de depuración.
6. **Verifica la conexión:**
   - El ESP32 se conectará a WiFi y luego a SinricPro.
   - En el monitor serie verás "Connected to SinricPro".
7. **Vincula SinricPro con Alexa o Google Home:**
   - Abre la aplicación de Alexa o Google Home.
   - Busca la habilidad "SinricPro" y actívala.
   - Inicia sesión con tus credenciales de SinricPro.
   - Los dispositivos creados (LED 1, LED 2, LED 3) aparecerán en la lista de dispositivos.
8. **Prueba el control por voz:**
   - Di: "Alexa, enciende LED 1" o "Ok Google, apaga LED 2".
   - Observa cómo los LEDs responden al comando.
   - También puedes usar la aplicación de SinricPro desde el móvil para controlar los dispositivos manualmente.

## Posibles Mejoras

- Agregar más dispositivos (hasta 10 switches o incluir otros tipos como termostatos, ventiladores, etc.).
- Implementar control de brillo (dimmer) utilizando PWM y el tipo de dispositivo "Dimmer" en SinricPro.
- Añadir sensores (temperatura, humedad, movimiento) y visualizarlos en el panel de SinricPro.
- Configurar rutinas y automatizaciones en Alexa o Google Home (ej. "Apagar todas las luces al salir").
- Utilizar la funcionalidad de "escenas" de SinricPro para activar varios dispositivos con un solo comando.
- Agregar un botón físico para control manual como respaldo.

## Autor

Pichardo Rico Cristian Eduardo
