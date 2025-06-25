
// display.cpp
#include "display.h"
#include <Arduino.h>
#include <Wire.h>

// Constructor - Configura LCD I2C (20x4)
Display::Display(uint8_t lcd_addr, uint8_t columnas, uint8_t filas) 
: lcd(lcd_addr, columnas, filas), columnas(columnas), filas(filas) {
}  // Usa LiquidCrystal_I2C

void Display::begin() {
  Wire.begin();      // Inicia I2C
  lcd.init();          // Inicializa el LCD
  lcd.backlight();     // Enciende retroiluminación
  lcd.clear(); 
   // Mensaje de inicio centrado
    String mensaje = "Sistema Inicio";
    int espacio = (columnas - mensaje.length()) / 2;
    espacio = max(0, espacio); // Asegura que no sea negativo
    
    lcd.setCursor(espacio, 0);
    lcd.print(mensaje);

    #ifdef ARDUINO
    Serial.println("[LCD] Inicializado correctamente");
    Serial.printf("Tamaño: %dx%d\n", columnas, filas);
    #endif
}

void Display::mostrarDatos(float temperatura, float humedad) {
    // Línea 1: Temperatura
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatura, 1);
    lcd.print(" C");
    // Limpia el resto de la línea
    for(int i = String(temperatura, 1).length() + 7; i < columnas; i++) {
        lcd.print(' ');
    }
    
    
    // Línea 2: Humedad
    lcd.setCursor(0, 1);
    lcd.print("    Hum:  ");
    lcd.print(humedad, 1);
    lcd.print(" %");
    // Limpia el resto de la línea
    for(int i = String(humedad, 1).length() + 7; i < columnas; i++) {
        lcd.print(' ');
    }
}
void Display::mostrarMensaje(int fila, int columna, String texto) {
    // Validación de posición
    fila = constrain(fila, 0, this->filas-1);
    columna = constrain(columna, 0, this->columnas-1);
    
    lcd.setCursor(columna, fila);
    lcd.print(texto);
    
    // Limpieza del resto de la línea
    int remaining = columnas - (columna + texto.length());
    if(remaining > 0) {
        for(int i = 0; i < remaining; i++) {
            lcd.print(' ');
        }
    }

    #ifdef ARDUINO
    Serial.printf("[LCD] Mensaje en (%d,%d): ", fila, columna);
    Serial.println(texto);
    #endif
}

void Display::mostrarEstadosActuadores(bool calefactor, bool humidificador, bool ventilador) {
    if(filas >= 3) {
        // Para displays con 3+ filas (20x4)
        lcd.setCursor(0, 2);
        lcd.print("C: ");
        lcd.print(calefactor ? "ON " : "OFF ");
        lcd.print("V:  ");
        lcd.print(ventilador ? "ON " : "OFF ");
        lcd.print("H: ");
        lcd.print(humidificador ? "ON " : "OFF ");
        for(int i = 16; i < columnas; i++) lcd.print(' ');
    } 
  }    
       void Display::mostrarIP(String ip) {
    // Fila 4: Dirección IP
    lcd.setCursor(0, 3);
    lcd.print("IP:");
    lcd.print(ip.substring(0, columnas-3)); // Asegura que no desborde
    
    // Limpieza de residuales
    for(int i = ip.length()+3; i < columnas; i++) lcd.print(' ');
  
  }
