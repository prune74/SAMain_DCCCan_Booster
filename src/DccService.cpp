#include "DccService.h"
#include "DccDecoder.h"
#include "CanBooster.h"
#include "pins.h"
#include "Config.h"

// ---------------------------------------------------------------------------
//  TÂCHE DCC (peu de travail, l’ISR fait tout)
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
//  TÂCHE CAN (DCC → CAN Booster + failsafe)
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

            // Sortie du failsafe
            if (failsafeActive)
            {
                failsafeActive = false;
                CanBooster_sendTelemetry(0, 0, BOOSTER_OK);
            }

            // Traitement des événements DCC
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

            // LED debug
            digitalWrite(PIN_LED, !digitalRead(PIN_LED));
        }
        else
        {
            uint32_t now = millis();

            // Entrée en failsafe
            if (!failsafeActive && (now - lastDccMs > DCCB_FAILSAFE_TIMEOUT_MS))
            {
                failsafeActive = true;
                failsafeSince = now;

                CanBooster_sendCutout(true, true);
                CanBooster_sendTelemetry(0, 0, BOOSTER_OFF);
            }

            // Sortie du failsafe si signal revenu
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
