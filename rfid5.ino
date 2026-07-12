// ------------------------------------------------------------
// Proyecto: Lectura de tarjetas/tag NFC con PN532 y ESP32 (I2C)
// Autor: Pedro (Mecatrónica - IoT)
// ------------------------------------------------------------

#include <Wire.h>
#include <Adafruit_PN532.h>
#include <WiFi.h>
#include "time.h"

// Pines I2C ESP32
#define SDA_PIN 21
#define SCL_PIN 22

// LEDs y buzzer
#define LED_AZUL 2
#define LED_ROJO 4
#define BUZZER 15

// Inicialización PN532 en I2C
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// UIDs autorizados
byte UID_ZAGAL[] = {0x42, 0x21, 0x29, 0x07};
String NOMBRE_ZAGAL = "Zagal";

byte UID_PEDRO[] = {0xC2, 0x60, 0x12, 0x07};
String NOMBRE_PEDRO = "Pedro";

// WiFi para NTP
const char* ssid = "ITT_D3";
const char* password = "todossomostecnm";

// Servidor NTP
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

  // Conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");

  // Configuración NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  Serial.println("Escanea una tarjeta o tag NFC...");
}

bool compararUID(byte *uidLeido, uint8_t uidLength, byte *uidAutorizado, uint8_t lenAutorizado) {
  if (uidLength != lenAutorizado) return false;
  for (uint8_t i = 0; i < uidLength; i++) {
    if (uidLeido[i] != uidAutorizado[i]) return false;
  }
  return true;
}

void loop() {
  uint8_t uid[7];
  uint8_t uidLength;

  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    return;
  }

  // Mostrar UID detectado
  Serial.print("UID detectado: ");
  for (uint8_t i = 0; i < uidLength; i++) {
    Serial.print("0x");
    Serial.print(uid[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Comparar con lista de autorizados
  bool autorizado = false;
  String nombre = "Desconocido";

  if (compararUID(uid, uidLength, UID_ZAGAL, sizeof(UID_ZAGAL))) {
    autorizado = true;
    nombre = NOMBRE_ZAGAL;
  } else if (compararUID(uid, uidLength, UID_PEDRO, sizeof(UID_PEDRO))) {
    autorizado = true;
    nombre = NOMBRE_PEDRO;
  }

  // Obtener fecha y hora actual
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error obteniendo hora");
    return;
  }

  char fecha[20], hora[20];
  strftime(fecha, sizeof(fecha), "%Y-%m-%d", &timeinfo);
  strftime(hora, sizeof(hora), "%H:%M:%S", &timeinfo);

  // Registrar acceso en monitor serial
  if (autorizado) {
    Serial.printf("Registro: {\"nombre\":\"%s\",\"fecha\":\"%s\",\"hora\":\"%s\",\"estado\":\"Autorizado\"}\n",
                  nombre.c_str(), fecha, hora);
    digitalWrite(LED_AZUL, HIGH);
    digitalWrite(LED_ROJO, LOW);
    noTone(BUZZER);
  } else {
    Serial.printf("Registro: {\"nombre\":\"%s\",\"fecha\":\"%s\",\"hora\":\"%s\",\"estado\":\"Rechazado\"}\n",
                  nombre.c_str(), fecha, hora);
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_ROJO, HIGH);
    tone(BUZZER, 1000);
    delay(500);
    noTone(BUZZER);
  }

  delay(2000);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_ROJO, LOW);
}
