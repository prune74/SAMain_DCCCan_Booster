#include "SystemInit.h"
#include "Settings.h"
#include "Wifi_fl.h"
#include "WebHandler.h"
#include "DccDecoder.h"
#include "CanBooster.h"
#include "Cli.h"
#include "CanService.h"

extern Fl_Wifi wifi;
extern WebHandler webHandler;

void systemInit()
{
    // Chargement des paramètres
    Settings::begin();
    Settings::readFile();

    // WiFi + Web
    wifi.start();
    webHandler.init(80);

    // DCC + Booster
    DccDecoder_begin();
    CanBooster_begin();
    Cli_begin();

    // CAN Service (MCP2515)
    CanService::begin();
}
