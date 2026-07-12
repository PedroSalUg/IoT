#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Credenciales WiFi
const char* ssid = "Legomadre";
const char* password = "Ugarte00";

// Configuración del sensor DHT
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Servidor web en el puerto 80
AsyncWebServer server(80);

// Función para leer temperatura
String readTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Error al leer la temperatura");
    return "";
  }
  return String(t);
}

// Función para leer humedad
String readHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Error al leer la humedad");
    return "";
  }
  return String(h);
}

// Reemplaza los marcadores en HTML
String processor(const String& var) {
  if (var == "TEMPERATURE") return readTemperature();
  else if (var == "HUMIDITY") return readHumidity();
  return String();
}

// Página HTML con estilo y actualización automática
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Sensor DHT11</title>
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" crossorigin="anonymous">
  <style>
    html { font-family: Arial; text-align: center; }
    h2 { font-size: 3rem; }
    p { font-size: 2rem; }
    .units { font-size: 1.2rem; }
    .dht-labels { font-size: 1.5rem; vertical-align: middle; padding-bottom: 15px; }
  </style>
</head>
<body>
  <h2><i class="fas fa-thermometer-half"></i> Temperatura y Humedad</h2>
  <p><i class="fas fa-temperature-high"></i> Temperatura: <span id="temperature">%TEMPERATURE%</span> °C</p>
  <p><i class="fas fa-tint"></i> Humedad: <span id="humidity">%HUMIDITY%</span> %</p>

  <script>
    setInterval(function () {
      fetch('/temperature')
        .then(response => response.text())
        .then(data => {
          document.getElementById("temperature").innerHTML = data;
        });

      fetch('/humidity')
        .then(response => response.text())
        .then(data => {
          document.getElementById("humidity").innerHTML = data;
        });
    }, 10000); // Actualiza cada 10 segundos
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");

  // Ruta principal
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Rutas para AJAX
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", readTemperature());
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", readHumidity());
  });

  server.begin();
}

void loop() {
  // No se requiere código en loop para este servidor
}
