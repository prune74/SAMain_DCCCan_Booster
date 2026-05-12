#ifndef __WEBHANDLER_H__
#define __WEBHANDLER_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "Settings.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

class WebHandler
{
protected:
    AsyncWebServer *_server;
    AsyncWebSocket *_ws;

    // Gestion des événements WebSocket
    void _WsEvent(AsyncWebSocket *server,
                  AsyncWebSocketClient *client,
                  AwsEventType type,
                  void *arg,
                  uint8_t *data,
                  size_t len);

public:
    WebHandler();

    // Initialisation du serveur Web
    void init(uint16_t webPort);

    // Boucle Web optimisée (cleanup + auto-sleep)
    void loop();

    // Traitement des messages WebSocket
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

    // Envoi aux clients WebSocket (throttlé)
    void notifyClients();

    // Définition des routes HTTP
    void route();
};

#endif
