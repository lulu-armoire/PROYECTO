

// WebServerControl.cpp

#include "WebServerControl.h"
#include <WebServer.h>
#include <WiFi.h>
#include <Arduino.h>



WebServerControl::WebServerControl() : servidor(80) {
    temperaturaActual = 0.0;
    humedadActual = 0.0;

    calefactorEstado = false;
    calefactorManual = false;
    humidificadorEstado = false;
    humidificadorManual = false;
    ventiladorEstado = false;
    ventiladorManual = false;
}


void WebServerControl::comenzar(const char* ssid, const char* password) {
    _ssid = ssid;
    _password = password;
    
    // Configura WiFi y rutas del servidor 
    WiFi.begin(_ssid,_password);
    Serial.print("[WiFi] Conectando a ");
    Serial.println(_ssid);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n[WiFi] Conectado. IP: " + WiFi.localIP().toString());

    servidor.on("/", HTTP_GET, [this]() {
        String htm1 = "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
                      "<style>"
                      "body { font-family: Arial; background-color: #f0f8ff; color: #333; padding: 20px;max-width: 600px;margin: 0 auto; }"
                      "h1 { color: #2c3e50;text-align: center;}"
                      "h2 { color: #2980b9;text-align: center;  }"
                      "p { font-size: 18px;text-align: center; }"
                      ".dato { font-weight: bold; animation: cambioColor 2s infinite; }"
                      ".nombre { font-weight: bold; }"
                      "@keyframes cambioColor {"
                      "0% { color: #e74c3c; }"
                      "50% { color: #27ae60; }"
                      "100% { color: #e74c3c; }"
                      "}"
                      "</style></head><body>"
                      "<h1>Monitor del Invernadero</h1>"
                      "<p><span class='nombre'>Temperatura:</span> <span class='dato'>" + String(temperaturaActual, 2) + " &deg;C</span></p>"
                      "<p><span class='nombre'>Humedad:</span> <span class='dato'>" + String(humedadActual, 2) + " %</span></p>"
                      "<h2>Estado de los Actuadores</h2>"
                      "<p><span class='nombre'>Calefactor:</span> " + String(calefactorEstado ? "ON" : "OFF") + " (" + String(calefactorManual ? "Manual" : "Automatico") + ")</p>"
                      "<p><span class='nombre'>Humidificador:</span> " + String(humidificadorEstado ? "ON" : "OFF") + " (" + String(humidificadorManual ? "Manual" : "Automatico") + ")</p>"
                      "<p><span class='nombre'>Ventilador:</span> " + String(ventiladorEstado ? "ON" : "OFF") + " (" + String(ventiladorManual ? "Manual" : "Automatico") + ")</p>"
                      "</body></html>";

        servidor.send(200, "text/html", htm1);
    });

    servidor.begin();
    Serial.println("[WebServer] Servidor HTTP iniciado.");
}

bool WebServerControl::estaConectado() {
    return WiFi.status() == WL_CONNECTED;
}

void WebServerControl::manejarCliente() {
    servidor.handleClient();
}

void WebServerControl::actualizarDatos(float temperatura, float humedad) {
    temperaturaActual = temperatura;
    humedadActual = humedad;
    Serial.print("[WebServer] Datos actualizados.");
}


void WebServerControl::actualizarEstados(bool cEstado, bool cManual,
                                         bool hEstado, bool hManual,
                                         bool vEstado, bool vManual) {
    calefactorEstado = cEstado;
    calefactorManual = cManual;
    humidificadorEstado = hEstado;
    humidificadorManual = hManual;
    ventiladorEstado = vEstado;
    ventiladorManual = vManual;
}
