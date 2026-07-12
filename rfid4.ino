/***************************************************
 * Configuración obligatoria para Blynk IoT
 ***************************************************/
#define BLYNK_TEMPLATE_ID "TMPL2y3xVk6uN"
#define BLYNK_TEMPLATE_NAME "Zagal"  // Reemplaza con el nombre de tu plantilla
#define BLYNK_AUTH_TOKEN "NEAa8ktZBEfBLLUJn0um01KE_5Gy4eRS"   // Reemplaza con tu Auth Token

#include <Wire.h>
#include <Adafruit_PN532.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Pines I2C ESP32
#define SDA_PIN 21
#define SCL_PIN 22

// LEDs y buzzer
#define LED_AZUL 2
#define LED_ROJO 4
#define BUZZER 15

// Inicialización PN532
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

// UID autorizado
byte UID_AUTORIZADO[] = {0x42, 0x21, 0x29, 0x07};

// Credenciales WiFi
char ssid[] = "ITT_D3";
char pass[] = "todossomostecnm";

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

  // Conexión a Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Escanea una tarjeta NFC...");
}

void loop() {
  Blynk.run();

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

    // Reflejar en la app Blynk
    Blynk.virtualWrite(V1, 1); // LED azul virtual
    Blynk.virtualWrite(V2, 0); // LED rojo virtual
    Blynk.virtualWrite(V3, 1); // acceso autorizado
  } else {
    Serial.println("ACCESO RECHAZADO");
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_ROJO, HIGH);
    tone(BUZZER, 1000);
    delay(500);
    noTone(BUZZER);

    Blynk.virtualWrite(V1, 0); // LED azul virtual
    Blynk.virtualWrite(V2, 1); // LED rojo virtual
    Blynk.virtualWrite(V3, 0); // acceso rechazado
  }

  delay(2000);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_ROJO, LOW);
}
