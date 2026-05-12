#ifndef __WEBHANDLER_H__
#define __WEBHANDLER_H__

#include "Arduino.h"
#include <ArduinoJson.h>
#include "Config.h"
#include "Settings.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h> // https://github.com/me-no-dev/ESPAsyncWebServer
#include <SPIFFS.h>


class WebHandler
{
protected:
   AsyncWebServer *_server;
   AsyncWebSocket *_ws;
   void _WsEvent(AsyncWebSocket*, AsyncWebSocketClient*, AwsEventType, void*, uint8_t*, size_t);

public:
   WebHandler();
   void init(uint16_t webPort);
   void loop();
   void handleWebSocketMessage(void*, uint8_t*, size_t);
   void notifyClients();
   void route();
};



#endif
