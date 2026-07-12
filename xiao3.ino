// --- CREDENCIALES DE BLYNK ---
// (Copia estos datos desde tu panel web de Blynk, deben ir siempre al inicio)
#define BLYNK_TEMPLATE_ID "TMPL2y3xVk6uN"
#define BLYNK_TEMPLATE_NAME "Zagal"
#define BLYNK_AUTH_TOKEN "NEAa8ktZBEfBLLUJn0um01KE_5Gy4eRS"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// --- CREDENCIALES WIFI ---
char ssid[] = "ITT_D3";
char pass[] = "todossomostecnm";

// Definir pines
const int buttonPin = D9;   
const int ledPin = D8;      // LED controlado por botón físico
const int ledPinV2 = D10;   // LED controlado por botón virtual V2

// Variables para el estado del botón
int lastButtonState = HIGH; 
int currentButtonState;     

// Estado del LED virtual en V1
int virtualLedState = 0; 

// Variables para el "debounce"
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50;  

void setup() {
  Serial.begin(115200);

  // Inicializar conexión con Blynk y WiFi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Configurar pines
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPinV2, OUTPUT);

  // Asegurarnos de que el LED virtual en V1 inicie apagado
  Blynk.virtualWrite(V1, 0); 
}

// --- Handler para botón virtual en V2 ---
BLYNK_WRITE(V2) {
  int value = param.asInt(); // 1 = ON, 0 = OFF
  digitalWrite(ledPinV2, value ? HIGH : LOW);
  Serial.printf("LED en D10 (V2): %s\n", value ? "ON" : "OFF");
}

void loop() {
  // Mantiene viva la conexión con los servidores de Blynk
  Blynk.run(); 

  // --- LÓGICA DEL BOTÓN FÍSICO ---
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != currentButtonState) {
      currentButtonState = reading;

      // Solo actuamos si el botón físico fue PRESIONADO (LOW)
      if (currentButtonState == LOW) {
        
        // Invertimos el estado del LED virtual en V1
        if (virtualLedState == 0) {
          virtualLedState = 255; // brillo máximo
        } else {
          virtualLedState = 0;   // apagado
        }

        // Enviamos la instrucción al PIN VIRTUAL V1 en Blynk
        Blynk.virtualWrite(V1, virtualLedState); 
      }
    }
  }

  // Guardar la lectura actual para la próxima vuelta
  lastButtonState = reading;
}
