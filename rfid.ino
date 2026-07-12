// ------------------------------------------------------------
// Proyecto: Mostrar UID de tarjetas/tag NFC con PN532 y ESP32 (I2C)
// Autor: Pedro (Mecatrónica - IoT)
// ------------------------------------------------------------

#include <Wire.h>
#include <Adafruit_PN532.h>

// Pines I2C ESP32
#define SDA_PIN 21
#define SCL_PIN 22

// Inicialización PN532 en I2C
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("No se encontró el PN532");
    while (1);
  }

  nfc.SAMConfig();

  Serial.println("Escanea una tarjeta o tag NFC...");
}

void loop() {
  uint8_t uid[7];       // buffer para UID
  uint8_t uidLength;    // longitud del UID

  // Intentar leer tarjeta/tag
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
    Serial.print("UID detectado: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print("0x");
      Serial.print(uid[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    delay(1000); // pequeña pausa antes de volver a leer
  }
}

