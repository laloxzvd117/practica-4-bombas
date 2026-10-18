# Practica: Control y Monitoreo de 4 bombas

Sistema embebido de control y supervisión industrial desarrollado con **dos microcontroladores ESP32** (Maestro y Esclavo) comunicados vía UART (Serial2), diseñado para la gestión en tiempo real y protección de sobrecorriente ("Guardamotor") para un banco de 4 bombas de agua, complementado con un script de Python para el registro de datos en tiempo real.

## 🚀 Características Principales

* **Arquitectura Distribuida:**
  * **ESP32 Maestro:** Encargado de la adquisición de señales analógicas (simulación de corriente con potenciómetros), lectura de entradas digitales (DIP Switches de control manual), actuación sobre los relés y envío de telemetría por USB.
  * **ESP32 Esclavo:** Dedicado exclusivamente a la interfaz visual, gestionando una pantalla LCD I2C (16x2) y una matriz de 12 LEDs indicadores de estado por bomba.

* **Protección por Sobrecorriente (Guardamotor/Relevador):** Lógica implementada por software utilizando `millis()` para evaluar condiciones de sobrecarga con un umbral de tolerancia de 2 segundos antes del disparo/corte de seguridad.

* **Comunicación Serial Robusta:** Enlace bidireccional asíncrono entre placas mediante Serial2 a 115200 baudios, y transmisión de datos de telemetría hacia un equipo de cómputo por puerto serial USB.

* **Registro en Python (`practica.py`):** Captura y almacenamiento automático de los estados de las 4 bombas desde el puerto serial.

## 🛠️ Requisitos e Instalación de Hardware

1. **Librerías necesarias en Arduino IDE:**
   * `LiquidCrystal_I2C` (para el control de la pantalla LCD).

2. **Alimentación eléctrica:**
   * Alimentar los módulos de relés directamente desde el pin **VIN / 5V** del ESP32 para evitar caídas de voltaje en la lógica de sensores.
   * Mantener una línea común de **GND** conectada físicamente entre ambas placas ESP32 y los módulos periféricos.

## 📊 Ejecución del Script de Registro en Python

Para capturar y registrar en tiempo real los valores de telemetría enviados por el ESP32 Maestro, se incluye el script `practica.py`. Es altamente recomendable ejecutarlo dentro de un entorno virtual para mantener las dependencias aisladas.

### 1. Crear y activar el entorno virtual

Abre tu terminal, navega a la carpeta de tu proyecto y ejecuta los siguientes comandos dependiendo de tu sistema operativo:

**En Windows:**
```bash
python -m venv venv
venv\Scripts\activate
```

**En Linux / macOS:**
```bash
python3 -m venv venv
source venv/bin/activate
```
*(Sabrás que está activado porque aparecerá el prefijo `(venv)` al inicio de tu línea de comandos).*

### 2. Instalar dependencias

Con el entorno virtual activado, instala la librería requerida para la comunicación con el puerto USB:
```bash
pip install pyserial
```

### 3. Configurar el puerto serial

Abre el archivo `practica.py` con tu editor de texto o código preferido y verifica que la variable del puerto serial coincida con el puerto donde está conectado tu ESP32 Maestro:
* **En Windows:** Suele ser `"COM3"`, `"COM4"`, etc.
* **En Linux / macOS:** Suele ser `"/dev/ttyUSB0"` o `"/dev/ttyACM0"`.

### 4. Ejecutar el script

Con el ESP32 Maestro conectado a tu computadora, inicia la captura de datos ejecutando:
```bash
python practica.py
```

El script comenzará a leer el puerto serial y desplegará en la consola el monitoreo de la corriente y las alertas del guardamotor de cada bomba.
