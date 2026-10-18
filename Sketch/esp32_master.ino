// Pines de los potenciómetros (Señal)
const int pot1 = 32; const int pot2 = 33; const int pot3 = 34; const int pot4 = 35;
// Pines de los interruptores (cables manuales a GND)
const int sw1 = 25; const int sw2 = 26; const int sw3 = 27; const int sw4 = 14;
// Pines de los relevadores (Control físico de las bombas)
const int rel1 = 13; const int rel2 = 23; const int rel3 = 18; const int rel4 = 19;

// Pines para comunicación con el Esclavo (Serial2)
#define RX_PIN 16
#define TX_PIN 17

// Variables para el control de tiempo (Guardamotor de 2 segundos)
unsigned long tiempoFalla[4] = {0, 0, 0, 0};
bool bloqueado[4] = {false, false, false, false};

void setup() {
  Serial.begin(115200); 
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); 
  
  pinMode(sw1, INPUT_PULLUP); pinMode(sw2, INPUT_PULLUP);
  pinMode(sw3, INPUT_PULLUP); pinMode(sw4, INPUT_PULLUP);

  pinMode(rel1, OUTPUT); pinMode(rel2, OUTPUT);
  pinMode(rel3, OUTPUT); pinMode(rel4, OUTPUT);
  
  // Apagar todos los relés por seguridad al iniciar
  digitalWrite(rel1, LOW); digitalWrite(rel2, LOW);
  digitalWrite(rel3, LOW); digitalWrite(rel4, LOW);
}

// Función maestra de seguridad y control
char procesarBomba(int indice, int pinSw, int pinPot, int pinRel) {
  bool encendida = (digitalRead(pinSw) == LOW);
  int corriente = analogRead(pinPot);

  // 1. Si apagas manualmente con el DIP switch (o reseteas una falla)
  if (!encendida) {
    bloqueado[indice] = false; // Quita el bloqueo
    tiempoFalla[indice] = 0;   // Reinicia el cronómetro
    digitalWrite(pinRel, LOW); // Corta la corriente física del relé
    return 'R'; // Marca como Apagada (Rojo)
  }

  // 2. Si la bomba "se botó" por el Guardamotor
  if (bloqueado[indice]) {
    digitalWrite(pinRel, LOW); // Mantiene cortada la corriente del relé
    return 'R'; // Mantiene en Rojo hasta que bajes el switch
  }

  // 3. Operación normal (Deja pasar corriente a la bomba)
  digitalWrite(pinRel, HIGH); 

  if (corriente > 3000) {
    if (tiempoFalla[indice] == 0) {
      tiempoFalla[indice] = millis(); // Inicia el cronómetro de advertencia
    }
    
    // Revisa si ya pasaron 2 segundos (2000 ms) en sobrecorriente
    if (millis() - tiempoFalla[indice] >= 2000) {
      bloqueado[indice] = true;  // Bloquea el sistema
      digitalWrite(pinRel, LOW); // Apaga el relé de inmediato
      return 'R'; // Pasa a Rojo
    } else {
      return 'Y'; // Sigue contando, se mantiene en Amarillo
    }
  } else {
    // Si la corriente baja antes de los 2 segundos, se cancela la alerta
    tiempoFalla[indice] = 0; 
    return 'G'; // Normal (Verde)
  }
}

void loop() {
  char e1 = procesarBomba(0, sw1, pot1, rel1);
  char e2 = procesarBomba(1, sw2, pot2, rel2);
  char e3 = procesarBomba(2, sw3, pot3, rel3);
  char e4 = procesarBomba(3, sw4, pot4, rel4);

  // Enviamos los estados al Esclavo para los LEDs y Pantalla
  Serial2.print(e1); Serial2.print(",");
  Serial2.print(e2); Serial2.print(",");
  Serial2.print(e3); Serial2.print(",");
  Serial2.println(e4); 
  
  // Enviamos los valores reales por USB para tu script de Python
  Serial.print("POT 1: "); Serial.print(e1 == 'R' ? "OFF" : String(analogRead(pot1)));
  Serial.print(" | POT 2: "); Serial.print(e2 == 'R' ? "OFF" : String(analogRead(pot2)));
  Serial.print(" | POT 3: "); Serial.print(e3 == 'R' ? "OFF" : String(analogRead(pot3)));
  Serial.print(" | POT 4: "); Serial.println(e4 == 'R' ? "OFF" : String(analogRead(pot4)));

  // Bucle a 200ms para asegurar la precisión del cronómetro de 2 segundos
  delay(200); 
}
