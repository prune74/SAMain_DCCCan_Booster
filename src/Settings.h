/*


*/

#ifndef __SETTINGS__
#define __SETTINGS__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <FS.h>
#include "Config.h"

class Settings
{
public:
  static bool WIFI_ON;
  static bool DISCOVERY_ON;
  static uint16_t idNode;
  static uint8_t nbLoco;
  static const byte tableLoco[NB_LOCOS][6];

  Settings() = delete;
  static void begin();
  static void readFile();
  static void writeFile();
};

#endif
