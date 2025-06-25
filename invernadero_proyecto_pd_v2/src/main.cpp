// ==============================
//           main.cpp
// ==============================

// Este programa lee un valor analógico de un sensor (AHT10) y enciende o apaga un actuador
// dependiendo de si el valor supera un umbral definido.

// ======= Librerías ===========
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <LiquidCrystal_I2C.h>
#include "actuador.h"
#include "webservercontrol.h"
#include "sensor.h"
#include "display.h"
#include <WiFi.h>
#include <WebServer.h>

// ======= Definición de Pines =======
#define PIN_SDA 8
#define PIN_SCL 9
#define BTN_CALEFACTOR 14
#define BTN_HUMIDIFICADOR 3
#define BTN_VENTILADOR 7
#define PIN_CALEFACTOR 5
#define PIN_HUMIDIFICADOR 4
#define PIN_VENTILADOR 6

// ======= Direcciones I2C =======
#define LCD_ADDRESS 0x27
#define SENSOR_ADDRESS 0x38

// ======= Configuración WiFi =======
const char* ssid = "Nautilus";
const char* password = "20000Leguas";

// ======= Instancias de Objetos =======
Display pantalla(LCD_ADDRESS);
Sensor miSensor(PIN_SDA, PIN_SCL);
WebServerControl servidorWeb;

Actuador calefactor(PIN_CALEFACTOR);
Actuador humidificador(PIN_HUMIDIFICADOR);
Actuador ventilador(PIN_VENTILADOR);

// ======= Variables Globales =======
float temperatura = 0.0;
float humedad = 0.0;
bool modoManual[3] = {false, false, false}; // [calefactor, humidificador, ventilador]

unsigned long ultimaLectura = 0;
const long intervaloLectura = 1000;

unsigned long ultimaActualizacionLCD = 0;
const int intervaloActualizacionLCD = 300; // ms

// ========================================
//        Funciones Auxiliares
// ========================================

// Escanea el bus I2C y muestra los dispositivos encontrados
void escanearI2C() {
  Serial.println("\nEscaneando bus I2C...");
  byte dispositivos = 0;
  
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.printf("Dispositivo encontrado en: 0x%02X", addr);
      if (addr == SENSOR_ADDRESS) Serial.print(" (Sensor ATH10)");
      if (addr == LCD_ADDRESS) Serial.print(" (Pantalla LCD)");
      Serial.println();
      dispositivos++;
    }
  }
  if (dispositivos == 0){
     Serial.println("¡No se detectaron dispositivos I2C!");
  } else {
     Serial.printf("Total encontrados: %d\n\n", dispositivos);
  }
}


// Gestiona el cambio manual del estado de un actuador desde su botón
void actualizarModoManual(int boton, bool& modo, Actuador& actuador) {
  static unsigned long ultimoCambio[3] = {0, 0, 0};
  int indice = (&actuador == &calefactor) ? 0 : ((&actuador == &humidificador) ? 1 : 2);
  unsigned long ahora = millis();
  
  if(digitalRead(boton) == LOW && (ahora - ultimoCambio[indice]) > 500) {
    ultimoCambio[indice] = ahora;
    
    bool estadoAnterior = actuador.leer();
    actuador.leer() ? actuador.OFF() : actuador.ON();
    modo = true;
    
    // Actualización inmediata del LCD
    if(estadoAnterior != actuador.leer() && (ahora - ultimaActualizacionLCD) > intervaloActualizacionLCD) {
      pantalla.mostrarEstadosActuadores(calefactor.leer(), 
                                      humidificador.leer(), 
                                      ventilador.leer());
       ultimaActualizacionLCD = ahora;
    }
    
    Serial.printf("[Botón] %s cambiado a %s (MANUAL)\n", 
                &actuador == &calefactor ? "Calefactor" : 
                (&actuador == &humidificador ? "Humidificador" : "Ventilador"),
                actuador.leer() ? "ON" : "OFF");
  }
}

// Comprueba si el actuador en modo manual coincide con la lógica automática
// y lo devuelve a modo automático si es así
void verificarCoincidenciaAutomatica() {
  bool cambios = false;
  
  // Calefactor (20°C)
  bool deberiaEstar = temperatura < 20.0;
  if(modoManual[0] && calefactor.leer() == deberiaEstar) {
    modoManual[0] = false;
    cambios = true;
    Serial.println("[Modo] Calefactor → AUTO (coincide)");
  }
  
  // Humidificador (50%)
  deberiaEstar = humedad < 50.0;
  if(modoManual[1] && humidificador.leer() == deberiaEstar) {
    modoManual[1] = false;
    cambios = true;
    Serial.println("[Modo] Humidificador → AUTO (coincide)");
  }
  
  // Ventilador (28°C)
  deberiaEstar = temperatura > 28.0;
  if(modoManual[2] && ventilador.leer() == deberiaEstar) {
    modoManual[2] = false;
    cambios = true;
    Serial.println("[Modo] Ventilador → AUTO (coincide)");
  }

  if(cambios) {
    pantalla.mostrarEstadosActuadores(calefactor.leer(),humidificador.leer(),ventilador.leer());
  }
}

// ========================================
//               SETUP
// ========================================

void setup() {
  Serial.begin(115200);
  while(!Serial);

  // Inicialización I2C y escaneo
  Wire.begin(PIN_SDA, PIN_SCL);
  escanearI2C();

  // Inicialización LCD
  pantalla.begin();
  pantalla.mostrarMensaje(0, 0, "Iniciando sistema...");

  // Inicialización sensor
  miSensor.begin();

  if(!miSensor.estaConectado()) {
    Serial.println("[Error] Sensor AHT10 no detectado!");
    pantalla.mostrarMensaje(1, 0, "Error sensor AHT10");
    while(1);
  }
  Serial.println("[Sistema] Sensor AHT10 inicializado");

  // Inicialización WiFi
  Serial.print("[WiFi] Conectando a ");
  Serial.println(ssid);
  pantalla.mostrarMensaje(2, 0, "Conectando WiFi...");
  
  servidorWeb.comenzar(ssid, password);
  
  Serial.print("[WiFi] Conectado! IP: ");
  Serial.println(WiFi.localIP());
  pantalla.mostrarMensaje(2, 0, "WiFi OK          ");
  pantalla.mostrarMensaje(3, 0, "IP: " + WiFi.localIP().toString());

  // Configuración de pines
  pinMode(BTN_CALEFACTOR, INPUT_PULLUP);
  pinMode(BTN_HUMIDIFICADOR, INPUT_PULLUP);
  pinMode(BTN_VENTILADOR, INPUT_PULLUP);

  // Estado inicial de actuad
  calefactor.OFF();
  humidificador.OFF();
  ventilador.OFF();

  Serial.println("[Sistema] Inicialización completada\n");
  delay(2000);
}

// ========================================
//                LOOP
// ========================================

void loop() {
  // 1. Lectura periódica de sensores
  if (millis() - ultimaLectura >= intervaloLectura) {
    ultimaLectura = millis();

    temperatura = miSensor.leerTemperatura();
    humedad = miSensor.leerHumedad();

     pantalla.mostrarDatos(temperatura, humedad);

       // 2.Control automático de actuadores
    if(!modoManual[0]) temperatura < 20.0 ? calefactor.ON() : calefactor.OFF();
    if(!modoManual[1]) humedad < 50.0 ? humidificador.ON() : humidificador.OFF();
    if(!modoManual[2]) temperatura > 28.0 ? ventilador.ON() : ventilador.OFF();

   

     
     // 3. Comprobación de coincidencias para volver a modo automático
    verificarCoincidenciaAutomatica();

    // 4. Mostrar estado de actuadores
    pantalla.mostrarEstadosActuadores(calefactor.leer(), humidificador.leer(), ventilador.leer());
    
    
    //5. Actualizar servidor web
    servidorWeb.actualizarDatos(temperatura, humedad);
    
    servidorWeb.actualizarEstados(
  calefactor.leer(), modoManual[0],
  humidificador.leer(), modoManual[1],
  ventilador.leer(), modoManual[2]
); 

 // 6.Debug por puerto serie
   Serial.println("\n--- ESTADO ACTUAL ---");
   Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Temp: %.1f°C | Hum: %.1f%%\n", temperatura, humedad);
    Serial.printf("Calefactor: %s (%s)\n", 
                calefactor.leer() ? "ON" : "OFF", 
                modoManual[0] ? "MANUAL" : "AUTO");
    Serial.printf("Humidificador: %s (%s)\n", 
                humidificador.leer() ? "ON" : "OFF", 
                modoManual[1] ? "MANUAL" : "AUTO");
    Serial.printf("Ventilador: %s (%s)\n", 
                ventilador.leer() ? "ON" : "OFF", 
                modoManual[2] ? "MANUAL" : "AUTO");
    Serial.println("---------------------");
  }

  // 7.Comprobación de botones manuales
  actualizarModoManual(BTN_CALEFACTOR, modoManual[0], calefactor);
  actualizarModoManual(BTN_HUMIDIFICADOR, modoManual[1], humidificador);
  actualizarModoManual(BTN_VENTILADOR, modoManual[2], ventilador);
  
  //8. Manejar peticiones web 
  servidorWeb.manejarCliente();
}