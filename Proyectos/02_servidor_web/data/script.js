/*
  script.js - Lógica de cliente para la interfaz web del Proyecto IoT 2
  Autor: Eduardo Pichardo
  Fecha: [03/03/2026]

  Descripción:
  Este archivo maneja la interacción del usuario con la página web del servidor ESP32.
  Utiliza peticiones AJAX (XMLHttpRequest) para comunicarse con el servidor sin recargar la página.
  Funciones:
    - toggleCheckbox: Enciende/apaga los LEDs 1-3 mediante peticiones GET a /update.
    - setInterval: Actualiza los valores de los sensores cada segundo consultando /sensorData.
    - updateSliderPWM: Ajusta el brillo del LED4 mediante el slider y envía el valor a /slider.
    - updateSliderPWM2: Ajusta el brillo del LED5 mediante el slider y envía el valor a /slider2.
*/

//Función que se ejecuta al cambiar el estado de un checkbox (LEDs 1, 2 o 3)
function toggleCheckbox(element) {
  //Crear objeto XMLHttpRequest para enviar petición asíncrona
  var xhr = new XMLHttpRequest();
  
  //Construir la URL con los parámetros: 
  //output = id del elemento (ej. "led1") y state = 1 si está marcado, 0 si no
  xhr.open("GET", "/update?output=" + element.id + "&state=" + (element.checked ? "1" : "0"), true);
  
  //Enviar la petición al servidor
  xhr.send();
}

//Temporizador que se ejecuta cada 1000 ms (1 segundo) para actualizar los sensores
setInterval(function() {
  var xhr = new XMLHttpRequest();
  
  //Solicitar los datos de los sensores al servidor
  xhr.open("GET", "/sensorData", true);
  
  //Definir función que se ejecuta cuando cambia el estado de la petición
  xhr.onreadystatechange = function() {
    //readyState 4 = operación completada, status 200 = respuesta OK
    if (xhr.readyState == 4 && xhr.status == 200) {
      //La respuesta es un string con dos valores separados por coma: "potValue,photoValue"
      var sensorData = xhr.responseText.split(',');
      
      //Actualizar el contenido de los elementos span con los nuevos valores
      document.getElementById('potValue').innerText = sensorData[0];   //Valor del potenciómetro
      document.getElementById('photoValue').innerText = sensorData[1]; //Valor de la fotoresistencia
    }
  };
  
  //Enviar la petición
  xhr.send();
}, 1000); //Intervalo de 1 segundo

//Función para controlar el brillo del LED4 mediante el slider
function updateSliderPWM(element) {
  //Obtener el valor actual del slider (0-255)
  var sliderValue = element.value;
  
  //Si el elemento que disparó la función es el slider con id "4"
  if (element.id === "4") {
    //Actualizar el texto del span que muestra el valor numérico del slider
    document.getElementById("textSliderValue").innerHTML = sliderValue;
  }
  
  //Enviar el valor al servidor mediante petición GET a /slider
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value=" + sliderValue, true);
  xhr.send();
}

//Función para controlar el brillo del LED5 mediante el slider (similar a la anterior)
function updateSliderPWM2(element) {
  var sliderValue2 = element.value;
  
  if (element.id === "5") {
    document.getElementById("textSliderValue5").innerHTML = sliderValue2;
  }
  
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider2?value5=" + sliderValue2, true);
  xhr.send();
}