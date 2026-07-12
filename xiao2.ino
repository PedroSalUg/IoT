// Definir los pines
const int ledPin = D8;     // Pin donde está conectado el LED
const int buttonPin = D9;   // Pin donde conectaremos el botón (puedes cambiarlo)

// Variables para recordar los estados
int ledState = LOW;         // Estado actual del LED (apagado por defecto)
int lastButtonState = HIGH; // Estado anterior del botón (HIGH por el pull-up)
int currentButtonState;     // Estado actual leído del botón

// Variables para el "debounce" (evitar rebotes falsos del botón)
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50;   // 50 milisegundos de espera

void setup() {
  // Configurar el pin del LED como salida
  pinMode(ledPin, OUTPUT);
  
  // Configurar el pin del botón como entrada con resistencia interna (Pull-up)
  // Al usar INPUT_PULLUP, el botón leerá HIGH normalmente, y LOW al presionarlo.
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Asegurarnos de que el LED inicie en el estado por defecto (apagado)
  digitalWrite(ledPin, ledState);
}

void loop() {
  // Leer el estado físico actual del botón
  int reading = digitalRead(buttonPin);

  // Si el estado del botón cambió (alguien lo tocó o hay ruido eléctrico)
  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // Reiniciar el temporizador
  }

  // Si ya pasó el tiempo de espera (50ms) y la señal es estable
  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    // Si el estado realmente cambió
    if (reading != currentButtonState) {
      currentButtonState = reading;

      // Solo cambiamos el estado del LED si el botón fue PRESIONADO (leemos LOW)
      if (currentButtonState == LOW) {
        ledState = !ledState; // Invierte el estado: si era HIGH pasa a LOW, y viceversa
      }
    }
  }

  // Encender o apagar el LED según el estado guardado
  digitalWrite(ledPin, ledState);

  // Guardar la lectura actual para la próxima vuelta del ciclo
  lastButtonState = reading;
}