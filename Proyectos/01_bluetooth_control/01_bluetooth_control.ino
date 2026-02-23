/*
  Proyecto IoT 1: Control Bluetooth con ESP32 y Sensor DHT11
  Autor: Eduardo Pichardo
  Fecha de última modificación: [22/02/2026]

  Descripción:
  Este programa permite controlar 5 LEDs a través de comandos Bluetooth
  y leer temperatura/humedad del sensor DHT11 mediante pulsadores físicos.
  Los comandos enviados por Bluetooth (1-5) encienden LEDs individuales,
  el comando 0 apaga todos. Al presionar SW1 se envía la temperatura,
  al presionar SW2 se envía la humedad.

  Conexiones:
  - LEDs: GPIO 14, 27, 26, 25, 33 (con resistencias 220Ω a GND)
  - SW1: GPIO 15 (pull-down externo de 10kΩ a GND, conmutador a VCC)
  - SW2: GPIO 4 (pull-up interno, conmutador a GND)
  - DHT11: GPIO 32 (VCC 3.3V, GND, Data)
*/

//Incluimos las librerías necesarias
#include "BluetoothSerial.h"  //Para comunicación Bluetooth
#include "DHT.h"  //Para el sensor DHT


//Verificar que el Bluetooth esté habilitado en la configuración del ESP32
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//Definición de pines 
#define LED1 14
#define LED2 27
#define LED3 26
#define LED4 25
#define LED5 33
#define SW1 15 //Pulsador con pull-down externo
#define SW2 4  //Pulsador con pull-up interno
#define DHTPIN 32      //Pin de datos del DHT11
#define DHTTYPE DHT11  //Tipo de sensor DHT

//Arreglo para manejar los LEDs fácilmente
const int leds[5] = {LED1, LED2, LED3, LED4, LED5};

//Objetos globales
BluetoothSerial BT; //Objeto para Bluetooth
DHT dht(DHTPIN, DHTTYPE); //Objetopara el sensor DHT

//Variables para antirrebote de los botones (tiempo en ms)
long timeCounter1 = 0;
long timeCounter2 = 0;
int timeThreshold = 250; //Tiempo mínimo entre lecturas

//Función callback que se ejecuta cuando ocurre un evento Bluetooth
void callback_function(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  switch (event) {
    case ESP_SPP_START_EVT:
      Serial.println("Inicializado SPP");
      break;
    case ESP_SPP_SRV_OPEN_EVT:
      Serial.println("Cliente conectado");
      break;
    case ESP_SPP_CLOSE_EVT:
      Serial.println("Cliente desconectado");
      break;
    case ESP_SPP_DATA_IND_EVT:
      //Mientras haya datos disponibles en el buffer Bluetooth
      while(BT.available()){ 
        int val = BT.read(); //Lee un byte
        //Ignorar caracteres de nueva línea o retorno de carro   
        if (val == '\n' || val == '\r') {
          continue; //Saltar a la siguiente iteración del bucle si es '\n' o '\r'
        }
        Serial.println(val); // Depuración: muestra el código ASCII recibido
        
        //Interpretar el comando (valores ASCII)
        if(val == 49) { //1 en ASCII
          digitalWrite(LED1, HIGH);
        }
        else if(val == 50){ //2 en ASCII
          digitalWrite(LED2, HIGH);
        }
        else if(val == 51){ //3 en ASCII
          digitalWrite(LED3, HIGH);
        }
        else if(val == 52){ //4 en ASCII
          digitalWrite(LED4, HIGH);
        }
        else if(val == 53){ //5 en ASCII
          digitalWrite(LED5, HIGH);
        }
        else if(val == 48){ //0 en ASCII
          //Apagar todos los LEDs
          for (int i = 0; i < 5 ; i++) {
            digitalWrite(leds[i], LOW);
          }
        }
        else{
          //Comando no reconocido
          BT.println("Error, ingrese un comando válido");
        }
      }
      break;
  }
}



void setup() {
  Serial.begin(115200); //Iniciar comunicación serie para depuración
  
 //Configurar todos los pines de los LEDs como salida
  for (int i = 0; i < 5 ; i++) {
    pinMode(leds[i], OUTPUT);
  }
  
  //Configurar pines de los pulsadores
  pinMode(SW1, INPUT);  //SW1 necesita pull-down externo, por eso INPUT
  pinMode(SW2, INPUT);  //SW2 usa resistencia pull-up interna
  
  //Inicializar Bluetooth con nombre visible
  BT.begin("Nombre_dispositivo"); //Nombre de su dispositivo Bluetooth y en modo esclavo
  Serial.println("El dispositivo Bluetooth está listo para emparejarse");
  
  //Registrar la función callback para manejar eventos Bluetooth
  BT.register_callback(callback_function); 

  //Inicializar el sensor DHT
  Serial.println(F("DHTxx test!"));
  dht.begin();

  
}



void loop() {  
  //Leer el estado del pulsador SW1 (con pull-down)
  if(digitalRead(SW1)==1){ //Pulsador presionado (activo alto)
    //Control de antirrebote por tiempo
    if (millis() > timeCounter1 + timeThreshold) {
      //Leer temperatura en Celsius y Fahrenheit
      float t=dht.readTemperature(); //°C
      float f=dht.readTemperature(true);  //°F

      //Verificar si la lectura fue exitosa
      if (isnan(t) || isnan(f)){
        BT.println(F("Error al leer la temperatura desde el sensor DHT"));
        return;
      }

      //Enviar datos por Bluetooth
      BT.print(F("Temperatura: "));
      BT.print(t);
      BT.print(F("°C "));
      BT.print(F("; "));
      BT.print(f);
      BT.println(F("°F"));

      //Actualizar tiempo para antirrebote
      timeCounter1 = millis();
    }
  }

   //Leer el estado del pulsador SW2 (con pull-up, activo bajo)
  if(digitalRead(SW2)==0){ //Pulsador presionado (conecta a GND)
    if (millis() > timeCounter2 + timeThreshold) {
      //Leer humedad relativa
      float h = dht.readHumidity();

      //Revisa si el sensor lee la humedad
      if(isnan(h)){
        BT.println(F("Error al leer la humedad desde el sensor DHT"));
        return;
      }
    
      BT.print(F("Humedad: "));
      BT.print(h);
      BT.println(F("%"));
      timeCounter2 = millis();
    }
  }
}
