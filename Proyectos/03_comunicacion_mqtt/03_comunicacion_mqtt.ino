/*
  Proyecto IoT 3: Comunicación MQTT con ESP32
  Autor: Pichardo Rico Cristian Eduardo
  Fecha de última modificación: [28/06/2026]

  Descripción:
  Este proyecto implementa un cliente MQTT en el ESP32 que se comunica con un broker (mosquitto)
  para publicar datos de sensores y recibir comandos para controlar LEDs.
  Los sensores incluyen: fotoresistencia (LDR), potenciómetro y DHT11 (temperatura/humedad).
  Además, se pueden controlar 5 LEDs mediante comandos numéricos enviados por MQTT.
  Los botones físicos permiten publicar los valores de los sensores de forma manual.
  El DHT11 publica automáticamente cada 5 segundos.

  Conexiones:
  - LED1: GPIO 14
  - LED2: GPIO 27
  - LED3: GPIO 26
  - LED4: GPIO 25
  - LED5: GPIO 33
  - Botón Pull-up: GPIO 4 (activo bajo)
  - Botón Pull-down: GPIO 15 (activo alto)
  - LDR: GPIO 34 (entrada analógica)
  - Potenciómetro: GPIO 35 (entrada analógica)
  - DHT11: GPIO 32
*/

// Bibliotecas necesarias
#include <EspMQTTClient.h>  // Cliente MQTT para ESP32
#include <DHT.h>             // Sensor DHT11

// Definición de pines
#define PinBotton_PullUp 4    // Botón 1 (pull-up interno)
#define PinBotton_PullDown 15 // Botón 2 (pull-down externo)
#define luz 34                // LDR (fotoresistencia)
#define pot 35                // Potenciómetro
#define led1 14               // Leds
#define led2 27
#define led3 26
#define led4 25
#define led5 33
#define pinDHT 32             // Pin de datos del DHT11
#define DHTTYPE DHT11         // Tipo de sensor DHT

// Arreglo con todos los LEDs para facilitar el manejo en bucles
const int leds[5] = {led1, led2, led3, led4, led5};

// Configuración de la red WiFi y del broker MQTT
const char* ssid = "Nombre_red";
const char* password = "Contraseña_red";
const char* broker = "test.mosquitto.org";  // Broker público de prueba
const char* nameClient = "ESP32_name";  // Identificador único del cliente
const int port = 1883;                       // Puerto estándar MQTT

// Tópicos MQTT (se recomienda una estructura jerárquica)
String topicSubLed = "ClaseIoT/Pichardo/Led";            // Suscripción para control de LEDs
String topicPubLuz = "ClaseIoT/Pichardo/Fotoresistencia"; // Publicación LDR
String topicPubPot = "ClaseIoT/Pichardo/Potenciometro";   // Publicación potenciómetro
String topicPubHum = "ClaseIoT/Pichardo/DHT/Humedad";     // Publicación humedad
String topicPubTemp = "ClaseIoT/Pichardo/DHT/Temperatura";// Publicación temperatura

// Variables para control de tiempo (antirrebote y temporización)
long timeCounter = 0;          // Almacena el tiempo de la última lectura por botón
const int timeThreshold = 350; // Umbral en ms para antirrebote
long timeDHT = 0;              // Almacena el tiempo de la última publicación del DHT

// Crear objeto para el sensor DHT
DHT dht(pinDHT, DHTTYPE);

// Crear objeto cliente MQTT (conexión automática)
EspMQTTClient client(ssid, password, broker, nameClient, port);

// Función que se ejecuta al establecer la conexión con el broker MQTT
void onConnectionEstablished() {
  // Suscripción al tópico para recibir comandos de LEDs
  client.subscribe(topicSubLed, [](const String & payload) {
    handleLedControl(payload);  // Llama al manejador con el payload recibido
  });
}

// Manejador de comandos para controlar LEDs
void handleLedControl(const String & payload) {
  int ledValue = payload.toInt();  // Convertir payload a entero

  // Encender un LED específico (1-5)
  if (ledValue == 1) {
    digitalWrite(led1, HIGH);
    Serial.println("Led 1 encendido");
  } 
  else if (ledValue == 2) {
    digitalWrite(led2, HIGH);
    Serial.println("Led 2 encendido");
  } 
  else if (ledValue == 3) {
    digitalWrite(led3, HIGH);
    Serial.println("Led 3 encendido");
  } 
  else if (ledValue == 4) {
    digitalWrite(led4, HIGH);
    Serial.println("Led 4 encendido");
  } 
  else if (ledValue == 5) {
    digitalWrite(led5, HIGH);
    Serial.println("Led 5 encendido");
  } 
  // Apagar todos los LEDs
  else if (ledValue == 0) {
    for (int i = 0; i < 5 ; i++) {
      digitalWrite(leds[i], LOW);
    }  
    Serial.println("Todos los leds se han apagado");
  }
  // Encender LEDs impares (1,3,5)
  else if (ledValue == 6) {
    for (int i = 0; i < 5 ; i++) {
      digitalWrite(leds[i], LOW);
    }  
    for (int i = 0; i < 5 ; i = i + 2) {
      digitalWrite(leds[i], HIGH);
    } 
    Serial.println("Leds impares encendidos");
  }
  // Encender LEDs pares (2,4)
  else if (ledValue == 7) {
    for (int i = 0; i < 5 ; i++) {
      digitalWrite(leds[i], LOW);
    }  
    for (int i = 1; i < 5 ; i = i + 2) {
      digitalWrite(leds[i], HIGH);
    } 
    Serial.println("Leds pares encendidos");
  }
  // Efecto "tren" ascendente (LEDs se encienden en secuencia)
  else if (ledValue == 8) {
    Serial.println("Tren encendido");
    for (int i = 0; i < 5 ; i++) {
      digitalWrite(leds[i], LOW);
    }  
    for (int i = 0; i < 5 ; i++) {
      digitalWrite(leds[i], HIGH);
      delay(800);
    } 
  }
  // Efecto "tren" descendente (LEDs se apagan en secuencia)
  else if (ledValue == 9) {
    Serial.println("Tren apagado");
    for (int i = 0; i < 5 ; i++) {
      digitalWrite(leds[i], HIGH);
    }  
    for (int i = 4; i >= 0 ; i--) {
      digitalWrite(leds[i], LOW);
      delay(800);
    } 
  }
  else {
    Serial.println("Valor no válido");
  }
}

void setup() {
  Serial.begin(115200);        // Iniciar comunicación serie
  dht.begin();                 // Inicializar sensor DHT

  // Configurar pines de sensores y botones
  pinMode(luz, INPUT);
  pinMode(pot, INPUT);
  pinMode(PinBotton_PullUp, INPUT_PULLUP);   // Botón con pull-up interno
  pinMode(PinBotton_PullDown, INPUT);        // Botón con pull-down externo

  // Configurar pines de LEDs como salida
  for (int i = 0; i < 5 ; i++) {
    pinMode(leds[i], OUTPUT);
  }

  // Verificar estado de la conexión MQTT (inicialmente no está conectado)
  if (!client.isConnected()) {
    Serial.println("¡Conectado con el broker!");
  } else {
    Serial.println("No Conectado, revise su conexión");
  }
  delay(5000);  // Pequeña pausa para estabilizar
}

void loop() {
  client.loop();  // Mantener viva la conexión MQTT y manejar mensajes entrantes

  // Lectura de los botones
  int in_up = digitalRead(PinBotton_PullUp);   // Activo bajo (LOW al presionar)
  int in_down = digitalRead(PinBotton_PullDown); // Activo alto (HIGH al presionar)

  // Botón Pull-up (activo bajo) -> publica valor de la fotoresistencia
  if (in_up == LOW) {
    if (millis() > timeCounter + timeThreshold) {
      int sensorLuz = analogRead(luz);
      client.publish(topicPubLuz, String(sensorLuz));
      Serial.print("El valor de la fotoresistencia es: ");
      Serial.println(sensorLuz);
      timeCounter = millis();
    }
  }

  // Botón Pull-down (activo alto) -> publica valor del potenciómetro
  if (in_down == HIGH) {
    if (millis() > timeCounter + timeThreshold) {
      int sensorPot = analogRead(pot);
      client.publish(topicPubPot, String(sensorPot));
      Serial.print("El valor del potenciometro es: ");
      Serial.println(sensorPot);
      timeCounter = millis();
    }
  }

  // Publicación automática del DHT11 cada 5 segundos
  if (millis() - timeDHT > 5000) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Error al leer el sensor DHT");
      return;  // Salir si hay error
    }

    // Publicar temperatura y humedad
    client.publish(topicPubTemp, String(t));
    client.publish(topicPubHum, String(h));

    Serial.print("Humedad: ");
    Serial.print(h);
    Serial.print(" % -- Temperatura: ");
    Serial.print(t);
    Serial.println(" °C");

    timeDHT = millis();
  }
}
