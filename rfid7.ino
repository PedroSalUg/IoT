#include <Wire.h>
#include <Adafruit_PN532.h>
#include <EEPROM.h>

#define RELAY_PIN   5
#define BUTTON_PIN  4
#define BUZZER_PIN  13
#define LED_STATUS  2

#define RELAY_ON  LOW
#define RELAY_OFF HIGH

#define MAX_TARJETAS 10
#define EEPROM_SIZE 128

Adafruit_PN532 nfc(21, 22);

unsigned long millisInicioLuz = 0;
const unsigned long TIEMPO_ESPERA = 5 * 60 * 1000;
bool luzEncendida = false;

byte tarjetas[MAX_TARJETAS][4];
int totalTarjetas = 0;

//-------------------------------
void setup() {
  Serial.begin(115200);

  // IMPORTANTE: apagar relay al inicio
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_STATUS, LOW);

  EEPROM.begin(EEPROM_SIZE);
  cargarTarjetas();

  nfc.begin();

  if (!nfc.getFirmwareVersion()) {
    Serial.println("PN532 no encontrado");
    while (1);
  }

  nfc.SAMConfig();
  Serial.println("Sistema listo");
}

//-------------------------------
void loop() {

  // BOTON REGISTRO
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(300);
    registrarNueva();
  }

  // AUTO APAGADO
  if (luzEncendida && millis() - millisInicioLuz >= TIEMPO_ESPERA) {
    apagarLuz();
  }

  uint8_t uid[7];
  uint8_t uidLength;

  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {

    if (esAutorizada(uid)) {
      Serial.println("Acceso permitido");
      toggleLuz();
    } else {
      Serial.println("Acceso denegado");
      tone(BUZZER_PIN, 500, 300);
    }

    delay(1000);
  }
}

//-------------------------------
void registrarNueva() {

  Serial.println("Modo registro");
  digitalWrite(LED_STATUS, HIGH);

  uint8_t uid[7];
  uint8_t uidLength;

  while (!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength));

  if (esAutorizada(uid)) {
    Serial.println("Ya registrada");
    tone(BUZZER_PIN, 1500, 200);
    digitalWrite(LED_STATUS, LOW);
    return;
  }

  if (totalTarjetas >= MAX_TARJETAS) {
    Serial.println("Memoria llena");
    tone(BUZZER_PIN, 500, 500);
    digitalWrite(LED_STATUS, LOW);
    return;
  }

  for (int i = 0; i < 4; i++) {
    tarjetas[totalTarjetas][i] = uid[i];
    EEPROM.write(totalTarjetas * 4 + i, uid[i]);
  }

  totalTarjetas++;
  EEPROM.write(100, totalTarjetas);
  EEPROM.commit();

  Serial.println("Tarjeta registrada");
  tone(BUZZER_PIN, 2000, 300);

  digitalWrite(LED_STATUS, LOW);
  delay(1000);
}

//-------------------------------
bool esAutorizada(byte *uid) {

  for (int i = 0; i < totalTarjetas; i++) {
    bool match = true;

    for (int j = 0; j < 4; j++) {
      if (tarjetas[i][j] != uid[j]) {
        match = false;
        break;
      }
    }

    if (match) return true;
  }

  return false;
}

//-------------------------------
void cargarTarjetas() {

  totalTarjetas = EEPROM.read(100);

  if (totalTarjetas > MAX_TARJETAS) {
    Serial.println("EEPROM corrupta, reiniciando...");
    totalTarjetas = 0;
    EEPROM.write(100, 0);
    EEPROM.commit();
  }

  for (int i = 0; i < totalTarjetas; i++) {
    for (int j = 0; j < 4; j++) {
      tarjetas[i][j] = EEPROM.read(i * 4 + j);
    }
  }

  Serial.print("Tarjetas cargadas: ");
  Serial.println(totalTarjetas);
}

//-------------------------------
void toggleLuz() {

  luzEncendida = !luzEncendida;

  digitalWrite(RELAY_PIN, luzEncendida ? RELAY_ON : RELAY_OFF);
  digitalWrite(LED_STATUS, luzEncendida);

  if (luzEncendida) {
    millisInicioLuz = millis();
    tone(BUZZER_PIN, 2000, 200);
  } else {
    tone(BUZZER_PIN, 1000, 200);
  }
}

//-------------------------------
void apagarLuz() {
  luzEncendida = false;
  digitalWrite(RELAY_PIN, RELAY_OFF);
  digitalWrite(LED_STATUS, LOW);
  tone(BUZZER_PIN, 800, 300);
}