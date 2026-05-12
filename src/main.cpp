/*

copyright (c) 2022 christophe.bobille - LOCODUINO - www.locoduino.org

*/

#ifndef ARDUINO_ARCH_ESP32
#error "Select an ESP32 board"
#endif

#define VERSION "v 0.3"
#define PROJECT "Main satellite"

#include "Config.h"
#include "Satellite.h"
#include "Settings.h"
#include "Task.h"
#include "WebHandler.h"
#include "Wifi_fl.h"

uint16_t idMain = 254;

// --- CAN SERVICE : MCP2515 ---
#include <SPI.h>
#include <ACAN2515.h>

static const uint32_t CAN_BITRATE = 250UL * 1000UL; // 250 Kb/s
static const byte MCP2515_CS  = 5;   // Chip Select
static const byte MCP2515_INT = 4;   // Interrupt

// ✔ Correct constructor for ACAN2515 v2.0.3
ACAN2515 canService(MCP2515_CS, SPI, MCP2515_INT);

Fl_Wifi wifi;
WebHandler webHandler;
Satellite sat[NB_SAT];

#define debug Serial

void setup()
{
  //--- Start serial
  debug.begin(115200);
  delay(100);

  Serial.printf("\n\nProject :    %s", PROJECT);
  Serial.printf("\nVersion :      %s", VERSION);
  Serial.printf("\nFichier :      %s", __FILE__);
  Serial.printf("\nCompiled :     %s", __DATE__);
  Serial.printf(" - %s\n\n", __TIME__);

  //--- Configure MCP2515 CAN Service
  debug.print("Configure MCP2515 CAN Service ");

  SPI.begin(18, 19, 23); // SCK, MISO, MOSI

  // ✔ Correct oscillator for ACAN2515 v2.0.3 (ancienne API)
  ACAN2515Settings settings(16UL * 1000UL * 1000UL, CAN_BITRATE);

  uint32_t errorCode = canService.begin(settings, [] { canService.isr(); });

  if (errorCode == 0)
    debug.print("ok !\n");
  else
    debug.printf("error 0x%x\n", errorCode);

  Settings::begin();
  Settings::readFile();

  wifi.start();
  webHandler.init(80);
}

void loop()
{
  CANMessage frameIn;
  CANMessage frameOut;

  if (canService.receive(frameIn))
  {
    debug.println("recu");
      
    auto sendMsg = [](CANMessage frameOut) -> bool
    {
      frameOut.id |= idMain;
      bool err = canService.tryToSend(frameOut);
      return err;
    };

    const byte priorite = (frameIn.id & 0x1E000000) >> 25;
    const byte cmde = (frameIn.id & 0x1FE0000) >> 17;
    const bool resp = (frameIn.id & 0x10000) >> 16;
    const uint16_t idExp = frameIn.id & 0xFFFF;
    
    frameOut.id |= priorite << 25;
    frameOut.ext = true;

    debug.printf("Reception du sattelite : %d\n", idExp);
    debug.printf("commande : 0x%0X\n", cmde);

    switch (cmde)
    {
    case 0xB2:
      debug.print("Req->Test du bus CAN\n");
      frameOut.len = 1;
      frameOut.id |= 0xB3 << 17;
      frameOut.id |= 0x01 << 16;
      frameOut.data[0] = 1;

      if (sendMsg(frameOut))
        debug.printf("Send->Test du bus CAN : OK\n\n");

      for (byte i = 0; i < NB_SAT; i++)
      {
        if (idExp == sat[i].id())
          break;
        else if (sat[i].id() == NO_ID)
        {
          sat[i].id(idExp);
          break;
        }
      }
      break;

    case 0xB4:
      if (Settings::idNode < 253)
      {
        debug.print("Req->Demande d'identifiant\n");
        frameOut.len = 1;
        frameOut.id |= 0xB5 << 17;
        frameOut.id |= 0x01 << 16;
        frameOut.data[0] = Settings::idNode;

        if (sendMsg(frameOut))
        {
          debug.printf("Send->Identifiant satellite : %d\n\n", Settings::idNode);
          Settings::idNode++;
          Settings::writeFile();
        }
      }
      else
        debug.printf("Taille maxi des identifiants atteinte : %d\n\n", Settings::idNode);
      break;
    }
  }
}
