
//webservercontrol.h

#ifndef WEBSERVERCONTROL_H
#define WEBSERVERCONTROL_H

#include <WebServer.h>
#include <WiFi.h>

class WebServerControl {
public:
    WebServerControl();  // Constructor sin parámetros
    void comenzar(const char* ssid, const char* password);  // Configura WiFi
    bool estaConectado();
    void manejarCliente();                                  // Gestiona peticiones HTTP
    void actualizarDatos(float temperatura, float humedad); // Actualiza valores
    void actualizarEstados(bool cEstado, bool cManual,
                       bool hEstado, bool hManual,
                       bool vEstado, bool vManual);
private:
    WebServer servidor;
    float temperaturaActual;  // Almacena valores localmente
    float humedadActual;
    const char* _ssid;     // Guardar credenciales
    const char* _password;

    bool calefactorEstado, calefactorManual;
    bool humidificadorEstado, humidificadorManual;
    bool ventiladorEstado, ventiladorManual;
};
#endif

