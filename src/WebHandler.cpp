// WebHandler.cpp

#include "WebHandler.h"
#include <ACAN_ESP32.h>

WebHandler::WebHandler() : _server(nullptr), _ws(nullptr) {}

void WebHandler::init(uint16_t webPort)
{
  _server = new AsyncWebServer(webPort);
  _ws = new AsyncWebSocket("/ws");
  _ws->onEvent(std::bind(&WebHandler::_WsEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

  WebHandler::route();

  _server->addHandler(_ws);
  _server->begin();
}

void WebHandler::loop()
{
  _ws->cleanupClients();
}

void WebHandler::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (strcmp((char *)data, "toggle") == 0)
    {
      WebHandler::notifyClients();
    }
  }
}

void WebHandler::_WsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;

  case WS_EVT_ERROR:
    //
    break;
  case WS_EVT_PONG:
    //
    break;
  case WS_EVT_DATA:
    // WebHandler::handleWebSocketMessage(arg, data, len);
    // Serial.printf("WebSocket arg %d\n", arg);
    // Serial.printf("WebSocket len %d\n", len);
    // Serial.printf("WebSocket data %s\n", data);

    StaticJsonDocument<1024> doc1; // Memory pool
    DeserializationError error = deserializeJson(doc1, data);

    if (error) // Check for errors in parsing
#ifdef DEBUG
      debug.println("Parsing failed");
#endif
    else
    {
      String message = (char *)data;

      if (message.indexOf("wifi_on") >= 0)
      {
        bool wifi_on = doc1["wifi_on"][0];
        Settings::WIFI_ON = wifi_on;

        CANMessage frame;
        frame.id |= 2 << 25;    // Priorite 0, 1 ou 2
        frame.id |= 0xBD << 17; // commande appelée
        frame.id |= 254;        // ID expediteur
        frame.ext = true;
        frame.len = 1;
        if(Settings::WIFI_ON)
        	frame.data[0] = 1;
    	else
    		frame.data[0] = 0;
        const bool ok = ACAN_ESP32::can.tryToSend(frame);
#ifdef DEBUG
        debug.printf(Settings::WIFI_ON ? "Wifi : on" : "Wifi : off");
        debug.printf("\n");
#endif
      }

      if (message.indexOf("discovery_on") >= 0)
      {
        bool discovery_on = doc1["discovery_on"][0];
        Settings::DISCOVERY_ON = discovery_on;

        CANMessage frame;
        frame.id |= 2 << 25;    // Priorite 0, 1 ou 2
        frame.id |= 0xBE << 17; // commande appelée
        frame.id |= 254;  // ID expediteur
        frame.ext = true;
        frame.len = 1;
        frame.data[0] = Settings::DISCOVERY_ON;
        const bool ok = ACAN_ESP32::can.tryToSend(frame);

#ifdef DEBUG
        debug.printf(Settings::DISCOVERY_ON ? "Discovery : on" : "Discovery : off");
        debug.printf("\n");
#endif
      }

      if (message.indexOf("save") >= 0)
      {
#ifdef DEBUG
        debug.println("save all");
#endif
        CANMessage frame;
        frame.id |= 2 << 25;    // Priorite 0, 1 ou 2
        frame.id |= 0xBF << 17; // commande appelée
        frame.id |= 254;  // ID expediteur
        frame.ext = true;
        frame.len = 0;
        const bool ok = ACAN_ESP32::can.tryToSend(frame);
      }

      if (message.indexOf("restartEsp") >= 0)
      {
#ifdef DEBUG
        debug.println("restartEsp");
#endif
        CANMessage frame;
        frame.id |= 2 << 25;    // Priorite 0, 1 ou 2
        frame.id |= 0xBC << 17; // commande appelée
        frame.id |= 254;  // ID expediteur
        frame.ext = true;
        frame.len = 0;
        const bool ok = ACAN_ESP32::can.tryToSend(frame);
      }
    }
    break;
  }
}

void WebHandler::notifyClients()
{
  _ws->textAll(String("ok"));
}

void WebHandler::route()
{
  // Route for root / web page
  _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });

  _server->on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/w3.css", "text/css"); });

  _server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/style.css", "text/css"); });

  _server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/script.js", "text/javascript"); });

  _server->on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/settings.json", "text/json"); });

  _server->on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/favicon.png", "image/png"); });

  _server->onNotFound([](AsyncWebServerRequest *request)
                      {
    Serial.printf("Not found: %s!\r\n", request->url().c_str());
    request->send(404); });
}
