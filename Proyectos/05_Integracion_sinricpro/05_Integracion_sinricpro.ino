/*
  Proyecto IoT 5: Control por Voz con SinricPro (Alexa / Google Home)
  Autor: Pichardo Rico Cristian Eduardo
  Fecha de última modificación: [28/06/2026]

  Descripción:
  Este proyecto integra el ESP32 con la plataforma SinricPro, que permite controlar
  dispositivos a través de comandos de voz utilizando Alexa o Google Home.
  Se definen tres interruptores (switches) que controlan tres LEDs (o relés)
  mediante la nube de SinricPro. El dispositivo se conecta a WiFi y se registra
  en la plataforma usando las credenciales de aplicación (APP_KEY y APP_SECRET)
  y los IDs de cada dispositivo creado en el portal de SinricPro.

  Conexiones:
  - LED1 (Switch 1): GPIO 14
  - LED2 (Switch 2): GPIO 27
  - LED3 (Switch 3): GPIO 26

  Funcionamiento:
  - Al recibir un comando de encendido/apagado desde la nube, se ejecuta la función
    callback correspondiente que cambia el estado del LED.
  - El bucle principal llama continuamente a SinricPro.handle() para mantener
    la comunicación con el servidor y procesar los mensajes entrantes.
*/

#ifdef ENABLE_DEBUG
  #define DEBUG_ESP_PORT Serial      // Puerto de depuración
  #define NODEBUG_WEBSOCKETS          // Desactiva depuración de WebSockets
  #define NDEBUG                      // Desactiva aserciones
#endif

#include <Arduino.h>                  // Funciones básicas de Arduino

// Incluir la biblioteca WiFi según la plataforma (ESP8266, ESP32 o RP2040)
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#endif

#include "SinricPro.h"               // Biblioteca principal de SinricPro
#include "SinricProSwitch.h"         // Para manejar dispositivos tipo switch


// Credenciales de la red WiFi
#define WIFI_SSID         "Nombre_red"
#define WIFI_PASS         "Clave_red"

// Credenciales de la aplicación SinricPro (obtenidas en el portal)
#define APP_KEY           "App_key"
#define APP_SECRET        "App_secret"

// IDs de los dispositivos (switches) creados en SinricPro
#define SWITCH_ID_1       "id_1"
#define RELAYPIN_1        14          // Pin para el LED1 (o relé)

#define SWITCH_ID_2       "id_2"
#define RELAYPIN_2        27          // Pin para el LED2

#define SWITCH_ID_3       "id_3"
#define RELAYPIN_3        26          // Pin para el LED3

#define BAUD_RATE         115200      // Velocidad de la comunicación serie

// Función callback para el Switch 1: se ejecuta cuando se recibe un cambio de estado
bool onPowerState1(const String &deviceId, bool &state) {
  Serial.printf("Device 1 turned %s", state ? "on" : "off");
  digitalWrite(RELAYPIN_1, state ? HIGH : LOW);   // Aplica el estado al LED
  return true;  // Indicar que el comando fue manejado correctamente
}

// Función callback para el Switch 2
bool onPowerState2(const String &deviceId, bool &state) {
  Serial.printf("Device 2 turned %s", state ? "on" : "off");
  digitalWrite(RELAYPIN_2, state ? HIGH : LOW);
  return true;
}

// Función callback para el Switch 3
bool onPowerState3(const String &deviceId, bool &state) {
  Serial.printf("Device 3 turned %s", state ? "on" : "off");
  digitalWrite(RELAYPIN_3, state ? HIGH : LOW);
  return true;
}

void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  // Configuraciones específicas según la plataforma
  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP);   // Evita que el WiFi entre en modo de bajo consumo
    WiFi.setAutoReconnect(true);
  #elif defined(ESP32)
    WiFi.setSleep(false);                 // Desactiva el modo de suspensión en ESP32
    WiFi.setAutoReconnect(true);
  #endif

  // Iniciar conexión a la red
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Esperar hasta que esté conectado
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }

  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

// 
void setupSinricPro() {
  // Configurar los pines de los LEDs como salida
  pinMode(RELAYPIN_1, OUTPUT);
  pinMode(RELAYPIN_2, OUTPUT);
  pinMode(RELAYPIN_3, OUTPUT);
  
  // Asociar cada switch con su ID y registrar el callback
  SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  mySwitch1.onPowerState(onPowerState1);
  
  SinricProSwitch& mySwitch2 = SinricPro[SWITCH_ID_2];
  mySwitch2.onPowerState(onPowerState2);
  
  SinricProSwitch& mySwitch3 = SinricPro[SWITCH_ID_3];
  mySwitch3.onPowerState(onPowerState3);
  
  // Configurar funciones de callback para eventos de conexión/desconexión
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
   
  // Iniciar la conexión con SinricPro usando las credenciales
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE);
  Serial.printf("\r\n\r\n");
  setupWiFi();         // Conectar a WiFi
  setupSinricPro();    // Configurar SinricPro
}

void loop() {
  // Mantener la conexión activa y procesar mensajes entrantes
  SinricPro.handle();
}
