#include <Arduino.h>
#include "pins.h"
#include "SystemInit.h"
#include "TasksSystem.h"
#include "CanBridge.h"
#include "Cli.h"
#include "Wifi_fl.h"
#include "WebHandler.h"

Fl_Wifi wifi;
WebHandler webHandler;

void setup()
{
    Serial.begin(115200);
    delay(200);

    Serial.println("\nSAMain_DCCCan_Booster v1.0\n");

    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);

    systemInit();
    startSystemTasks();
}

void loop()
{
    Cli_task();
    processCanService();
    vTaskDelay(pdMS_TO_TICKS(10));
}
