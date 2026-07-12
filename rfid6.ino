#include <Wire.h>
#include <Adafruit_PN532.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define LED_AZUL 2
#define LED_ROJO 4
#define BUZZER 15

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// UID autorizado
byte UID_PEDRO[] = {0x42, 0x21, 0x29, 0x07};
String NOMBRE_PEDRO = "Pedro";

// WiFi
const char* ssid = "ITT_D3";
const char* password = "todossomostecnm";

// Google Script URL
String scriptUrl = "https://script.google.com/macros/s/AKfycbzti7uXjNyCMy4-XWlSIE45ey0eu-r_jZQX_gnqirYI3_F4kvMrgLDgyxOhzXMq2dP1/exec";

// NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -21600; // México UTC-6
const int daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  nfc.begin();
  if (!nfc.getFirmwareVersion()) {
    Serial.println("No se encontró el PN532");
    while (1);
  }
  nfc.SAMConfig();

  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    return;
  }

  bool autorizado = true;
  String nombre = "Desconocido";

  if (uidLength == sizeof(UID_PEDRO)) {
    nombre = NOMBRE_PEDRO;
    for (byte i = 0; i < uidLength; i++) {
      if (uid[i] != UID_PEDRO[i]) {
        autorizado = false;
        nombre = "Desconocido";
        break;
      }
    }
  } else {
    autorizado = false;
  }

  // Fecha y hora
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error obteniendo hora");
    return;
  }
  char fecha[20], hora[20];
  strftime(fecha, sizeof(fecha), "%Y-%m-%d", &timeinfo);
  strftime(hora, sizeof(hora), "%H:%M:%S", &timeinfo);

  String estado = autorizado ? "Autorizado" : "Rechazado";

  // LEDs y buzzer
  if (autorizado) {
    digitalWrite(LED_AZUL, HIGH);
    digitalWrite(LED_ROJO, LOW);
    noTone(BUZZER);
  } else {
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_ROJO, HIGH);
    tone(BUZZER, 1000);
    delay(500);
    noTone(BUZZER);
  }

  // Enviar a Google Sheets
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(scriptUrl);
    http.addHeader("Content-Type", "application/json");

    String json = "{\"nombre\":\"" + nombre + "\",\"fecha\":\"" + fecha + "\",\"hora\":\"" + hora + "\",\"estado\":\"" + estado + "\"}";
    int httpResponseCode = http.POST(json);

    if (httpResponseCode > 0) {
      Serial.println("Registro enviado: " + json);
    } else {
      Serial.println("Error enviando registro");
    }
    http.end();
  }

  delay(2000);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_ROJO, LOW);


  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_ROJO, LOW);
  noTone(BUZZER);
}