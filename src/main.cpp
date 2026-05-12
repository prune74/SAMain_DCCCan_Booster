/*
   SAMain_DCCCan_Booster
   Fusion : Satellite Autonome Main + DCC_CAN-Booster
   Auteur : Bruno – Discovery 2026
*/

#ifndef ARDUINO_ARCH_ESP32
#error "Select an ESP32 board"
#endif

#define VERSION "v 1.0"
#define PROJECT "SAMain_DCCCan_Booster"

// ---------------------------------------------------------------------------
//  INCLUDES : SAMain
// ---------------------------------------------------------------------------
#include "Config.h"
#include "Satellite.h"
#include "Settings.h"
#include "Task.h"
#include "WebHandler.h"
#include "Wifi_fl.h"

// ---------------------------------------------------------------------------
//  INCLUDES : DCC_CAN-Booster
// ---------------------------------------------------------------------------
#include "pins.h"
#include "DccDecoder.h"
#include "CanBooster.h"
#include "Cli.h"

// ---------------------------------------------------------------------------
//  CAN SERVICE : MCP2515 (250 kbps)
// ---------------------------------------------------------------------------
#include <SPI.h>
#include <ACAN2515.h>


static const uint32_t CAN_SERVICE_BITRATE = 250UL * 1000UL;
static const byte MCP2515_CS  = 5;
static const byte MCP2515_INT = 4;

ACAN2515 canService(MCP2515_CS, SPI, MCP2515_INT);

// ---------------------------------------------------------------------------
//  GLOBALS
// ---------------------------------------------------------------------------
Fl_Wifi wifi;
WebHandler webHandler;
Satellite sat[NB_SAT];

uint16_t idMain = 254;

volatile bool canMonitorEnabled = false;
volatile int32_t canMonitorFilter = -1;

TaskHandle_t taskDccHandle = nullptr;
TaskHandle_t taskCanHandle = nullptr;
TaskHandle_t taskCanRxHandle = nullptr;

#define debug Serial

// ---------------------------------------------------------------------------
//  TÂCHES DCC_CAN-Booster (inchangées)
// ---------------------------------------------------------------------------
void taskDcc(void *pvParameters)
{
    for (;;) vTaskDelay(pdMS_TO_TICKS(10));
}

void taskCan(void *pvParameters)
{
    DccEvent ev;
    uint32_t lastDccMs = millis();
    uint32_t lastStatsMs = millis();

    bool failsafeActive = false;
    uint32_t failsafeSince = 0;

    for (;;)
    {
        if (DccDecoder_getEvent(ev))
        {
            lastDccMs = millis();

            if (failsafeActive)
            {
                failsafeActive = false;
                CanBooster_sendTelemetry(0, 0, BOOSTER_OK);
            }

            switch (ev.type)
            {
            case DCC_EVT_BIT:
                CanBooster_sendDccBit(ev.bit, ev.phase);
                break;

            case DCC_EVT_CUTOUT_START:
                CanBooster_sendCutout(true, true);
                break;

            case DCC_EVT_CUTOUT_END:
                CanBooster_sendCutout(false, false);
                break;
            }

            digitalWrite(PIN_LED, !digitalRead(PIN_LED));
        }
        else
        {
            uint32_t now = millis();

            if (!failsafeActive && (now - lastDccMs > DCCB_FAILSAFE_TIMEOUT_MS))
            {
                failsafeActive = true;
                failsafeSince = now;

                CanBooster_sendCutout(true, true);
                CanBooster_sendTelemetry(0, 0, BOOSTER_OFF);
            }

            if (failsafeActive && (now - failsafeSince > DCCB_FAILSAFE_COOLDOWN_MS))
            {
                if (now - lastDccMs < DCCB_FAILSAFE_TIMEOUT_MS)
                {
                    failsafeActive = false;
                    CanBooster_sendTelemetry(0, 0, BOOSTER_OK);
                }
            }

            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}

void taskCanRx(void *pvParameters)
{
    CANMessage msg;

    for (;;)
    {
        if (ACAN_ESP32::can.receive(msg))
        {
            // Passerelle Booster → CAN Service
            canService.tryToSend(msg);
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

// ---------------------------------------------------------------------------
//  SETUP
// ---------------------------------------------------------------------------
void setup()
{
    debug.begin(115200);
    delay(200);

    Serial.printf("\n\nProject : %s\nVersion : %s\n\n", PROJECT, VERSION);

    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);

    // -----------------------------------------------------------------------
    // 1) CAN SERVICE (MCP2515)
    // -----------------------------------------------------------------------
    debug.print("Configure MCP2515 CAN Service ");

    SPI.begin(18, 19, 23);

    ACAN2515Settings settings(16UL * 1000UL * 1000UL, CAN_SERVICE_BITRATE);
    uint32_t errorCode = canService.begin(settings, [] { canService.isr(); });

    if (errorCode == 0) debug.println("OK");
    else debug.printf("error 0x%x\n", errorCode);

    // -----------------------------------------------------------------------
    // 2) SAMain modules
    // -----------------------------------------------------------------------
    Settings::begin();
    Settings::readFile();

    wifi.start();
    webHandler.init(80);

    // -----------------------------------------------------------------------
    // 3) DCC_CAN-Booster
    // -----------------------------------------------------------------------
    DccDecoder_begin();
    CanBooster_begin();
    Cli_begin();

    // -----------------------------------------------------------------------
    // 4) FreeRTOS tasks
    // -----------------------------------------------------------------------
    xTaskCreatePinnedToCore(taskDcc,   "DCC",    4096, nullptr, 2, &taskDccHandle,   0);
    xTaskCreatePinnedToCore(taskCan,   "CAN",    4096, nullptr, 3, &taskCanHandle,   1);
    xTaskCreatePinnedToCore(taskCanRx, "CAN_RX", 4096, nullptr, 1, &taskCanRxHandle, 0);
}

// ---------------------------------------------------------------------------
//  LOOP : SAMain + CLI
// ---------------------------------------------------------------------------
void loop()
{
    // CLI Booster
    Cli_task();

    // -----------------------------------------------------------------------
    // 5) CAN SERVICE → CAN BOOSTER
    // -----------------------------------------------------------------------
    CANMessage frameIn;

    if (canService.receive(frameIn))
    {
        // Forward vers CAN Booster
        ACAN_ESP32::can.tryToSend(frameIn);

        // Gestion Discovery (inchangée)
        CANMessage frameOut;

        auto sendMsg = [](CANMessage frameOut) -> bool
        {
            frameOut.id |= idMain;
            return canService.tryToSend(frameOut);
        };

        const byte priorite = (frameIn.id & 0x1E000000) >> 25;
        const byte cmde     = (frameIn.id & 0x1FE0000) >> 17;
        const uint16_t idExp = frameIn.id & 0xFFFF;

        frameOut.id |= priorite << 25;
        frameOut.ext = true;

        switch (cmde)
        {
        case 0xB2:
            frameOut.len = 1;
            frameOut.id |= 0xB3 << 17;
            frameOut.id |= 0x01 << 16;
            frameOut.data[0] = 1;
            sendMsg(frameOut);

            for (byte i = 0; i < NB_SAT; i++)
            {
                if (idExp == sat[i].id()) break;
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
                frameOut.len = 1;
                frameOut.id |= 0xB5 << 17;
                frameOut.id |= 0x01 << 16;
                frameOut.data[0] = Settings::idNode;

                if (sendMsg(frameOut))
                {
                    Settings::idNode++;
                    Settings::writeFile();
                }
            }
            break;
        }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
}
