/*
  Proyecto IoT 2: Servidor Web con ESP32
  Autor: Eduardo Pichardo
  Fecha de última modificación: [03/03/2026]

  Descripción:
  Este programa implementa un servidor web asíncrono en un ESP32 que permite
  monitorear en tiempo real los valores de un potenciómetro y una fotoresistencia,
  así como controlar cinco LEDs: tres de ellos mediante interruptores (on/off)
  y dos mediante control de brillo PWM. Los archivos de la interfaz web (HTML,
  CSS, JavaScript) se almacenan en el sistema de archivos SPIFFS del ESP32.
  La comunicación con el cliente se realiza mediante peticiones AJAX para
  actualización en tiempo real sin recargar la página.

  Conexiones:
  - LED1: GPIO 14 (salida digital)
  - LED2: GPIO 27 (salida digital)
  - LED3: GPIO 26 (salida digital)
  - LED4: GPIO 25 (salida PWM, canal 0)
  - LED5: GPIO 33 (salida PWM, canal 1)
  - Potenciómetro: GPIO 35 (entrada analógica)
  - Fotoresistencia: GPIO 34 (entrada analógica)

  Todos los LEDs deben conectarse con resistencias limitadoras de 220Ω a GND.
*/

//Librerías necesarias
#include "WiFi.h"                //Conexión WiFi
#include "ESPAsyncWebServer.h"   //Servidor web asíncrono
#include "SPIFFS.h"              //Sistema de archivos en flash

//Definición de pines para los LEDs
const int led1 = 14;
const int led2 = 27;
const int led3 = 26;
const int led4 = 25;
const int led5 = 33;

//Variables para almacenar los valores de los sliders PWM (inician en 0)
String sliderValue = "0";
String sliderValue2 = "0";

//Nombres de los parámetros que se recibirán en las peticiones HTTP
const char* PARAM_INPUT_1 = "output";   //Identificador del LED
const char* PARAM_INPUT_2 = "state";    //Estado (0/1)

//Pines para los sensores analógicos
const int potPin = 35;           //Potenciómetro
const int photoResistorPin = 34;  //Fotoresistencia

//Variables para almacenar el estado de los LEDs y valores de sensores
int led1State = LOW;    //No se usa directamente, pero se podría emplear para sincronización
int led2State = LOW;
int led3State = LOW;
int potValue = 0;
int photoValue = 0;

//Propiedades PWM

//Configuración del PWM para LED4
const int freq = 1000;           //Frecuencia de 1 kHz
const int ledChannel = 0;         //Canal PWM 0
const int resolution = 8;         //Resolución de 8 bits (0-255)
const char* PARAM_INPUT = "value"; //Parámetro para el slider de LED4

//Configuración del PWM para LED5 (segundo canal independiente)
const int freq2 = 1000;
const int ledChannel2 = 1;
const int resolution2 = 8;
const char* PARAM_INPUT2 = "value5"; //Parámetro para el slider de LED5

//Creamos Objeto AsyncWebServer, puerto 80
AsyncWebServer server(80);

//Credenciales de la red WiFi (deben ser modificadas por el usuario)
const char* ssid = "Nombre de la RED wifi";
const char* password = "Contraseña de la RED";

//Función para conectar a WiFi
void setup_wifi() {
  WiFi.mode(WIFI_STA);            //Modo estación (cliente)
  WiFi.disconnect();              //Desconectar de cualquier conexión previa
  delay(100);
  Serial.println("WiFi en modo estación establecido");
  
  delay(10);
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  //Esperar hasta que se establezca la conexión
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());   //Mostrar IP asignada
  delay(2000);
}

//Función que procesa placeholders en el HTML antes de enviarlo al cliente
String processor(const String& var) {
  //Reemplazar el placeholder BUTTONPLACEHOLDER con los controles de los LEDs 1-3
  if (var == "BUTTONPLACEHOLDER") {
    String buttons = "";
    buttons += "<h4>LED 1</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"1\" " + outputState(led1) + "><span class=\"slider1\"></span></label>";
    buttons += "<h4>LED 2</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(led2) + "><span class=\"slider2\"></span></label>";
    buttons += "<h4>LED 3</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"3\" " + outputState(led3) + "><span class=\"slider1\"></span></label>";
    return buttons;
  }
  //Reemplazar SLIDERVALUE con el valor actual del slider (para mostrar en el HTML)
  if (var == "SLIDERVALUE") {
    return sliderValue;
  }
  return String();   // Si no hay placeholder, devolver cadena vacía
}

//Función auxiliar que devuelve "checked" si el LED está encendido (para mantener estado en el checkbox)
String outputState(int output) {
  if (digitalRead(output)) {
    return "checked";
  }
  else {
    return "";
  }
}

//Función que lee ambos sensores y devuelve una cadena con los valores separados por coma
String readSensorData() {
  potValue = analogRead(potPin);               //Lectura del potenciómetro (0-4095)
  photoValue = analogRead(photoResistorPin);   //Lectura de la fotoresistencia (0-4095)
  String sensorData = String(potValue) + "," + String(photoValue);
  return sensorData;
}

void setup() {
  //Iniciar comunicación serial
  Serial.begin(115200);
  
  //Conectar a WiFi
  setup_wifi();

  //Configurar los pines de los LEDs como salida
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);

  //Asegurar que todos los LEDs comiencen apagados
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);

  //Configurar PWM para LED4 (canal 0)
  ledcAttachChannel(led4, freq, resolution, ledChannel);
  ledcWrite(led4, sliderValue.toInt());   //Establecer valor inicial (0)

  //Configurar PWM para LED5 (canal 1)
  ledcAttachChannel(led5, freq2, resolution2, ledChannel2);
  ledcWrite(led5, sliderValue2.toInt());  //Valor inicial (0)
  
  //Inicializar SPIFFS (sistema de archivos) para servir los archivos web
  if (!SPIFFS.begin(true)) {
    Serial.println("Error al leer SPIFFS");
    return;
  }


  //Definir rutas del servidor web

  //Página principal: sirve el archivo index.html y aplica el procesador de placeholders
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  //Archivo CSS
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  //Archivo JS
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "text/js");
  });

  //Ruta para obtener datos de sensores (llamada AJAX)
  server.on("/sensorData", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", readSensorData());
  });

  //Ruta para actualizar el estado de los LEDs 1-3 (encender/apagar)
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String inputMessage1;
    String inputMessage2;
    
    //Obtener parámetros de la URL: /update?output=ledX&state=1/0
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();   //Ej: "led1"
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();   //"1" o "0"

      //Actualizar el LED correspondiente
      if (inputMessage1 == "led1") {
        digitalWrite(led1, inputMessage2.toInt());
        Serial.print(inputMessage1);
        Serial.print(": ");
        Serial.println(inputMessage2);
      } 
      else if (inputMessage1 == "led2") {
        digitalWrite(led2, inputMessage2.toInt());
        Serial.print(inputMessage1);
        Serial.print(": ");
        Serial.println(inputMessage2);
      } 
      else if (inputMessage1 == "led3") {
        digitalWrite(led3, inputMessage2.toInt());
        Serial.print(inputMessage1);
        Serial.print(": ");
        Serial.println(inputMessage2);
      }
    }

    //Responder con OK
    request->send(200, "text/plain", "OK");
  });
  
  //Ruta para controlar el brillo del LED4 (PWM) mediante slider
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();   //Valor del slider (0-255)
      sliderValue = inputMessage;                               //Actualizar variable global
      ledcWrite(led4, sliderValue.toInt());                     //Aplicar al LED4
    } 
    else {
      inputMessage = "Mensaje no enviado";
    }

    Serial.print("led4");
    Serial.print(": ");
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  //Ruta para controlar el brillo del LED5 (PWM)
  server.on("/slider2", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT2)) {
      inputMessage = request->getParam(PARAM_INPUT2)->value();  //Valor del slider (0-255)
      sliderValue2 = inputMessage;
      ledcWrite(led5, sliderValue2.toInt());
    } 
    else {
      inputMessage = "Mensaje no enviado";
    }
    
    Serial.print("led5");
    Serial.print(": ");
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  //Manejar rutas no encontradas (404)
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Error 404");
  });

  //Iniciar el servidor
  server.begin();
}

void loop() {
  //El servidor es asíncrono, no necesita código en el loop
  //Todo se maneja mediante callbacks y el núcleo del ESP32
}
