#include <Wire.h>
#include <Adafruit_PN532.h>
#include <WiFi.h>
#include <UbidotsEsp32Mqtt.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define LED_AZUL 2
#define LED_ROJO 4
#define BUZZER 15

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// UID autorizado
byte UID_AUTORIZADO[] = {0x42, 0x21, 0x29, 0x07};

// WiFi
const char* WIFI_SSID = "ITT_D1";
const char* WIFI_PASS = "todossomostecnm";

// Ubidots
const char* TOKEN = "BBUS-hw1FEHvK3xXVyFQKFAxGmkCFKddjvO";
const char* DEVICE_LABEL = "esp32";

Ubidots ubidots(TOKEN);

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

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");

  ubidots.setup();
  ubidots.reconnect();

  Serial.println("Escanea una tarjeta NFC...");
}

void loop() {
  ubidots.loop();

  uint8_t uid[7];
  uint8_t uidLength;
  if (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    return;
  }

  bool tarjeta_autorizada = true;
  for (byte i = 0; i < uidLength; i++) {
    if (i < sizeof(UID_AUTORIZADO) && uid[i] != UID_AUTORIZADO[i]) {
      tarjeta_autorizada = false;
    }
  }

  if (tarjeta_autorizada) {
    Serial.println("ACCESO AUTORIZADO");
    digitalWrite(LED_AZUL, HIGH);
    digitalWrite(LED_ROJO, LOW);
    noTone(BUZZER);
    ubidots.add("acceso", 1);  // enviar a Ubidots
  } else {
    Serial.println("ACCESO RECHAZADO");
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_ROJO, HIGH);
    tone(BUZZER, 1000);
    delay(500);
    noTone(BUZZER);
    ubidots.add("acceso", 0);
  }

  ubidots.publish(DEVICE_LABEL);

  delay(2000);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_ROJO, LOW);
}

