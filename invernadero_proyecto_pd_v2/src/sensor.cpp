
// Sensor.cpp

#include "sensor.h"
#include <Wire.h>

Sensor::Sensor(int sdaPin, int sclPin) : _sdaPin(sdaPin), _sclPin(sclPin){}

void Sensor::begin() {
  Wire.begin(_sdaPin, _sclPin);
  sensorConectado = aht.begin();
  
  if (!sensorConectado) {
    Serial.println("Error al iniciar el sensor AHT10");
  } else {
        Serial.println("[Sensor] AHT10 iniciado correctamente.");
    }
}

bool Sensor::estaConectado () const {
    return sensorConectado; // Para verificar en el setup()
}

float Sensor::leerTemperatura() {
  if (!sensorConectado) {
    Serial.println("Error: No hay sensor conectado.");
  return -1.0;
}
sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    Serial.print("[Sensor] Temperatura leída: ");
    Serial.print(temp.temperature);
    Serial.println(" °C");
    return temp.temperature;
}

float Sensor::leerHumedad() {
   if (!sensorConectado) {
   Serial.println("[Sensor] Error: No hay sensor conectado.");
         return -1.0;
    }

    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    Serial.print("[Sensor] Humedad leída: ");
    Serial.print(humidity.relative_humidity);
    Serial.println(" %");
    return humidity.relative_humidity;
 
}