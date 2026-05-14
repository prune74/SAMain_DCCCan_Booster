#include "TasksSystem.h"
#include "CanService.h"
#include "CanBooster.h"
#include "Settings.h"
#include "Globals.h"
#include "pins.h"

// Handles FreeRTOS
TaskHandle_t taskDccHandle  = nullptr;
TaskHandle_t taskCanHandle  = nullptr;
TaskHandle_t taskSaveHandle = nullptr;

// ---------------------------------------------------------------------------
//  TÂCHE DCC : lit les événements DCC (ISR → events)
// ---------------------------------------------------------------------------
void taskDcc(void *pvParameters)
{
    for (;;)
    {
        // Le DCC est géré par l’ISR + DccDecoder_getEvent()
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// ---------------------------------------------------------------------------
//  TÂCHE CAN BOOSTER : envoie les trames DCC vers le Booster
// ---------------------------------------------------------------------------
void taskCan(void *pvParameters)
{
    DccEvent ev;
    uint32_t lastDccMs = millis();

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

// ---------------------------------------------------------------------------
//  TÂCHE SAVE : demande périodique de sauvegarde aux satellites
// ---------------------------------------------------------------------------
void taskSave(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const uint32_t tempo = 60UL * 1000UL;

    for (;;)
    {
        for (byte i = 0; i < 20; i++)
        {
            /*
            CANMessage frameOut;
            frameOut.id = 0;
            frameOut.id |= 3 << 27;
            frameOut.id |= 254 << 19;
            frameOut.id |= i << 11;
            frameOut.id |= 0xBF << 3;
            frameOut.ext = true;
            frameOut.len = 0;

            CanService::send(frameOut);
            vTaskDelay(pdMS_TO_TICKS(10));*
            */
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(tempo));
    }
}

// ---------------------------------------------------------------------------
//  Lancement des tâches système
// ---------------------------------------------------------------------------
void startSystemTasks()
{
    xTaskCreatePinnedToCore(taskDcc,  "DCC", 4096, nullptr, 2, &taskDccHandle, 0);
    xTaskCreatePinnedToCore(taskCan,  "CAN", 4096, nullptr, 3, &taskCanHandle, 1);
    xTaskCreatePinnedToCore(taskSave, "SAVE", 3072, nullptr, 1, &taskSaveHandle, 0);
}
