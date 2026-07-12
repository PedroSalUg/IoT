#ifndef ZIGBEE_MODE_ED
#error "Selecciona 'Zigbee ED (End Device)' en el menú Herramientas > Zigbee mode"
#endif

#include "Zigbee.h"

#define LED_PIN D7 // Pin del LED

// Creamos una luz en el endpoint 10
ZigbeeLight zbLight = ZigbeeLight(D10);

void updateLED(bool state) {
  digitalWrite(LED_PIN, state ? HIGH : LOW);
  Serial.printf("LED estado: %s\n", state ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  zbLight.setManufacturerAndModel("Seeed", "XIAOled");
  // Registramos qué función debe ejecutarse cuando llegue un comando Zigbee
  zbLight.onLightChange(updateLED);

  Zigbee.addEndpoint(&zbLight);

  if (!Zigbee.begin(ZIGBEE_END_DEVICE)) {
    Serial.println("Error iniciando Dispositivo Final.");
    ESP.restart();
  }
}

void loop() {
  // El sistema de callbacks maneja todo, no necesitas nada aquí.
}
