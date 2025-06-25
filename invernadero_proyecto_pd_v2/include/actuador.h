
//Actuador.h

#ifndef ACTUADOR_H
#define ACTUADOR_H

#include <Arduino.h>

class Actuador {
public:
    Actuador(int pin);
    void ON();
    void OFF();
    bool leer();  // Devuelve el estado actual (encendido/apagado)

private:
    int pinActuador;
    bool estado;
};

#endif

