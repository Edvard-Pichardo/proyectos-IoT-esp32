/*
  Proyecto IoT 4: Control y Monitoreo con Telegram Bot
  Autor: Pichardo Rico Cristian Eduardo
  Fecha de última modificación: [28/06/2026]

  Descripción:
  Este proyecto implementa un bot de Telegram que permite controlar 5 LEDs,
  leer el valor de un potenciómetro y configurar una alarma basada en una fotoresistencia.
  El ESP32 se conecta a WiFi y utiliza la API de Telegram para recibir comandos
  y enviar respuestas. La alarma se activa cuando el nivel de luz cae por debajo
  de un umbral configurable por el usuario.

  Comandos disponibles:
  - /led [1-5] on/off    → Enciende o apaga un LED específico.
  - /pot                 → Devuelve la lectura actual del potenciómetro.
  - /rangoAlarma [0-4095] → Ajusta el umbral de la alarma de luz.
  - /alarma on/off       → Activa o desactiva el sistema de alarma.

  Conexiones:
  - LED1: GPIO 14
  - LED2: GPIO 27
  - LED3: GPIO 26
  - LED4: GPIO 25
  - LED5: GPIO 33
  - LDR (fotoresistencia): GPIO 34 (entrada analógica)
  - Potenciómetro: GPIO 35 (entrada analógica)
*/

// Bibliotecas necesarias
#include <WiFi.h>                  // Conexión WiFi
#include <UniversalTelegramBot.h>  // Cliente para la API de Telegram
#include <WiFiClientSecure.h>      // Conexión segura HTTPS

// Credenciales de red y bot
const char* ssid = "Nombre_red";
const char* password = "contraseña_red";
const char* botToken = "token_del_bot";  // Token proporcionado por BotFather

// Objetos para la comunicación con Telegram
WiFiClientSecure client;                 // Cliente seguro para HTTPS
UniversalTelegramBot bot(botToken, client); // Bot de Telegram

// Variables para el manejo de mensajes
String chat_id;                          // ID del chat del usuario que envía comandos
unsigned long contador;                  // Temporizador para consultar nuevos mensajes
const int botRequestDelay = 500;         // Intervalo entre consultas (ms)

// Definición de pines
#define led1 14
#define led2 27
#define led3 26
#define led4 25
#define led5 33
#define luz 34      // LDR
#define pot 35      // Potenciómetro

// Arreglo con todos los LEDs para facilitar el manejo en bucles
const int leds[5] = {led1, led2, led3, led4, led5};

// Variables para la alarma
int umbral_alarma = 500;   // Umbral inicial (0-4095). Si la luz baja de este valor, se dispara la alarma.
bool alarmEnabled = false; // Estado de la alarma (activada/desactivada)

void setup() {
  Serial.begin(115200);        // Iniciar comunicación serie

  // Configurar pines de sensores como entrada
  pinMode(luz, INPUT);
  pinMode(pot, INPUT);

  // Configurar pines de LEDs como salida y apagarlos inicialmente
  for (int i = 0; i < 5; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  // Conexión a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Conectando a: ");
    Serial.println(ssid);
  }
  Serial.println("Conectado a Wi-Fi");

  // Configuración para conexiones HTTPS inseguras (necesario para Telegram)
  // En ESP32, se usa setInsecure() para saltar la verificación del certificado.
  client.setInsecure();
}

// ------- MANEJADOR DE MENSAJES --------
void handleNewMessages(int numNewMessages) {
  // Recorrer todos los mensajes nuevos
  for (int i = 0; i < numNewMessages; i++) {
    chat_id = bot.messages[i].chat_id;         // Guardar ID del chat para responder
    String text = bot.messages[i].text;        // Texto del mensaje

    // ----- Control de LEDs: /led <número> on/off -----
    if (text.startsWith("/led")) {
      // Extraer el número del LED (carácter en posición 4)
      int ledNum = text.substring(4, 5).toInt();
      // Extraer la acción (a partir de la posición 6, separado por espacio)
      String action = text.substring(6);

      if (ledNum >= 1 && ledNum <= 5) {
        if (action == "on") {
          digitalWrite(leds[ledNum - 1], HIGH);
          bot.sendMessage(chat_id, "El led " + String(ledNum) + " está encendido", "");
          Serial.print("El led ");
          Serial.print(String(ledNum));
          Serial.println(" está encendido");
        } else if (action == "off") {
          digitalWrite(leds[ledNum - 1], LOW);
          bot.sendMessage(chat_id, "El led " + String(ledNum) + " está apagado", "");
          Serial.print("El led ");
          Serial.print(String(ledNum));
          Serial.println(" está apagado");
        } else {
          bot.sendMessage(chat_id, "Especifica un estado de encendido (on) o apagado (off)", "");
        }
      } else {
        bot.sendMessage(chat_id, "Especifica un LED entre 1 y 5", "");
      }
    }

    // ----- Lectura del potenciómetro: /pot -----
    if (text == "/pot") {
      int potValue = analogRead(pot);
      bot.sendMessage(chat_id, "El valor del potenciómetro es: " + String(potValue), "");
      Serial.print("El valor del potenciometro es ");
      Serial.println(potValue);
    }

    // ----- Configuración del umbral de alarma: /rangoAlarma <valor> -----
    if (text.startsWith("/rangoAlarma")) {
      // Extraer el valor después del comando (13 caracteres de longitud)
      int verificador = text.substring(13).toInt();

      // Validar que el valor esté en el rango permitido (0-4095)
      if (verificador < 0 || verificador > 4095) {
        bot.sendMessage(chat_id, "Error. Introduzca un valor entre 0 y 4095", "");
      } else {
        umbral_alarma = verificador;
        bot.sendMessage(chat_id, "El umbral de la alarma fue ajustado a " + String(umbral_alarma), "");
        Serial.print("Nuevo umbral de alarma: ");
        Serial.println(umbral_alarma);
      }
    }

    // ----- Activación y desactivación de la alarma -----
    if (text == "/alarma on") {
      alarmEnabled = true;
      bot.sendMessage(chat_id, "Alarma activada", "");
      Serial.println("Alarma activada");
    }
    if (text == "/alarma off") {
      alarmEnabled = false;
      bot.sendMessage(chat_id, "Alarma desactivada", "");
      Serial.println("Alarma desactivada");
    }
  }
}

void loop() {
  // Consultar nuevos mensajes cada botRequestDelay (500 ms)
  if (millis() > contador + botRequestDelay) {
    // Obtener actualizaciones (mensajes) desde la última vez
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    // Si hay mensajes nuevos, procesarlos
    if (numNewMessages) {
      handleNewMessages(numNewMessages);
    }

    contador = millis();  // Reiniciar temporizador
  }

  // ----- Monitoreo de la alarma (fotoresistencia) -----
  if (alarmEnabled) {
    int lightLevel = analogRead(luz);   // Leer nivel de luz (0-4095)

    // Si el nivel de luz es menor al umbral, disparar la alarma
    if (lightLevel < umbral_alarma) {
      // Enviar notificación por Telegram (solo si no se ha enviado repetidamente,
      // aquí se envía cada vez que se cumple la condición, pero se puede mejorar)
      bot.sendMessage(chat_id, "¡Alarma! Hay poca luz", "");
      Serial.println("¡Alarma activada! Hay poca luz");
      
      // Nota: Para evitar spam, se podría añadir una bandera para enviar solo una vez.
    }
  }
}
