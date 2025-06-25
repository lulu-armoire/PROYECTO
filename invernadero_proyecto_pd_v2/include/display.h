
// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include <LiquidCrystal_I2C.h>

class Display {
public:
    Display(uint8_t lcd_addr = 0x27,uint8_t columnas = 20, uint8_t filas = 4);  // Constructor con dirección I2C (0x27 por defecto)
    void begin();                      // Inicialización del display
    void mostrarDatos(float temperatura, float humedad);  // Muestra datos en el LCD
    void mostrarMensaje(int fila, int columna, String texto);  
    void mostrarEstadosActuadores(bool calefactor, bool humidificador, bool ventilador); 
    void mostrarIP(String ip);
      private:
    LiquidCrystal_I2C lcd;  // Objeto LCD con interfaz I2C
    uint8_t columnas;       // Añadimos para almacenar las dimensiones
    uint8_t filas;
};

#endif