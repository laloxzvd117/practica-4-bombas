#include <WiFi.h>
#include <time.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- CONFIGURACIÓN DE TU INTERNET ---
const char* ssid = "prueba2";
const char* password = "12345678912";

// Servidor de hora configurado para zona horaria de México (UTC -6)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600; 
const int   daylightOffset_sec = 0; 

// Pines de comunicación
#define RX_PIN 16
#define TX_PIN 17

// Mapa de Pines para los 12 LEDs
const int ledV1 = 13; const int ledA1 = 14; const int ledR1 = 27;
const int ledV2 = 26; const int ledA2 = 25; const int ledR2 = 15;
const int ledV3 = 32; const int ledA3 = 23; const int ledR3 = 18;
const int ledV4 = 19; const int ledA4 = 5;  const int ledR4 = 4;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  lcd.init();
  lcd.backlight();
  
  int pinesLeds[] = {ledV1, ledA1, ledR1, ledV2, ledA2, ledR2, ledV3, ledA3, ledR3, ledV4, ledA4, ledR4};
  for (int i = 0; i < 12; i++) {
    pinMode(pinesLeds[i], OUTPUT);
    digitalWrite(pinesLeds[i], LOW);
  }

  lcd.setCursor(0, 0);
  lcd.print("Conectando WiFi");
  WiFi.begin(ssid, password);
  int intentos = 0;
  
  // Limita los intentos para evitar congelamientos si no hay red
  while (WiFi.status() != WL_CONNECTED && intentos < 15) {
    delay(500);
    lcd.print(".");
    intentos++;
  }
  
  if(WiFi.status() == WL_CONNECTED){
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); 
    lcd.clear();
    lcd.print("WiFi OK!");
  } else {
    lcd.clear();
    lcd.print("Sin Internet");
  }
  delay(1500);
  lcd.clear();
}

void actualizarSemaforo(char estado, int pinV, int pinA, int pinR) {
  digitalWrite(pinV, estado == 'G' ? HIGH : LOW);
  digitalWrite(pinA, estado == 'Y' ? HIGH : LOW);
  digitalWrite(pinR, estado == 'R' ? HIGH : LOW);
}

String getEstadoCorto(char e) {
  if (e == 'G') return "OK"; 
  if (e == 'Y') return "AL"; 
  return "--";               
}

void loop() {
  // 1. EL RELOJ SIEMPRE SE ACTUALIZA (Sin importar si hay datos del Maestro)
  lcd.setCursor(0, 1);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    lcd.print("Hora no disp.   ");
  } else {
    char timeStr[17];
    strftime(timeStr, sizeof(timeStr), "%d/%m/%y %H:%M", &timeinfo);
    String tStr = String(timeStr);
    while(tStr.length() < 16) tStr += " "; 
    lcd.print(tStr);
  }

  // 2. PROCESAMIENTO DE DATOS DEL MAESTRO
  if (Serial2.available() > 0) {
    String datos = Serial2.readStringUntil('\n');
    datos.trim(); // Limpia saltos de línea y basura
    
    // Si la cadena tiene el tamaño correcto, la procesamos
    if (datos.length() >= 7) {
      char e1 = datos.charAt(0);
      char e2 = datos.charAt(2);
      char e3 = datos.charAt(4);
      char e4 = datos.charAt(6);
      
      actualizarSemaforo(e1, ledV1, ledA1, ledR1);
      actualizarSemaforo(e2, ledV2, ledA2, ledR2);
      actualizarSemaforo(e3, ledV3, ledA3, ledR3);
      actualizarSemaforo(e4, ledV4, ledA4, ledR4);
      
      bool hayFalla = (e1 == 'Y' || e2 == 'Y' || e3 == 'Y' || e4 == 'Y');
      
      lcd.setCursor(0, 0);
      if (hayFalla) {
        String fallaMsg = "PARO: ";
        if(e1 == 'Y') fallaMsg += "B1 ";
        if(e2 == 'Y') fallaMsg += "B2 ";
        if(e3 == 'Y') fallaMsg += "B3 ";
        if(e4 == 'Y') fallaMsg += "B4";
        fallaMsg.trim(); 
        
        while(fallaMsg.length() < 16) fallaMsg += " ";
        lcd.print(fallaMsg);
      } else {
        String normalMsg = "1" + getEstadoCorto(e1) + " 2" + getEstadoCorto(e2) + " 3" + getEstadoCorto(e3) + " 4" + getEstadoCorto(e4);
        while(normalMsg.length() < 16) normalMsg += " ";
        lcd.print(normalMsg);
      }
    }
  }
}
