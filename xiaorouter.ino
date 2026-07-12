#ifndef ZIGBEE_MODE_ZCZR
#error "Selecciona 'Zigbee ZCZR (Router)' en el menú Herramientas > Zigbee mode"
#endif

#include "Zigbee.h"

#define LED_PIN D7 // Pin físico del LED en el router

// Creamos una luz en el endpoint 10
ZigbeeLight zbLight = ZigbeeLight(10);

void updateLED(bool state) {
  digitalWrite(LED_PIN, state ? HIGH : LOW);
  Serial.printf("LED (Router) estado: %s\n", state ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Configuración del endpoint de luz
  zbLight.setManufacturerAndModel("Seeed", "XIAOrouterLED");
  zbLight.onLightChange(updateLED);

  Zigbee.addEndpoint(&zbLight);

  // Iniciar como Router
  if (!Zigbee.begin(ZIGBEE_ROUTER)) {
    Serial.println("Error iniciando Router Zigbee.");
    ESP.restart();
  }

  Serial.println("Router listo. Extiende la red y acepta comandos.");
}

void loop() {
  // El sistema de callbacks maneja todo, no necesitas lógica aquí
}
