
// Actuador.cpp

#include "actuador.h"

Actuador::Actuador(int pin) {
    pinActuador = pin;
    estado = false;
    pinMode(pinActuador, OUTPUT);
    digitalWrite(pinActuador, LOW); // Asegurarse de que empiece apagado
}

void Actuador::ON() {
    digitalWrite(pinActuador, HIGH);
    estado = true;
}

void Actuador::OFF() {
    digitalWrite(pinActuador, LOW);
    estado = false;
}

bool Actuador::leer() {
    return estado;
}

