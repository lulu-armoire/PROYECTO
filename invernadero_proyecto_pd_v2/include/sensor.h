
// Sensor.h

#ifndef SENSOR_H
#define SENSOR_H

#include <Wire.h>
#include <Adafruit_AHTX0.h>

class Sensor {
  public:
    Sensor(int sdaPin, int sclPin);
    void begin();
    bool estaConectado() const;             // Para verificar conexión
    float leerTemperatura();
    float leerHumedad();
    
  private:
    int _sdaPin;
    int _sclPin;
    Adafruit_AHTX0 aht;
    bool sensorConectado = false; // Para verificar si el sensor está conectado
};

#endif