#include "CanBridge.h"
#include "CanService.h"
#include "CanBooster.h"
#include "Globals.h"
#include "Settings.h"
#include <ACAN_ESP32.h>

// ---------------------------------------------------------------------------
//  Passerelle CAN Service (MCP2515) → CAN Booster (ACAN_ESP32)
//  + Gestion Discovery (commandes B2 / B4)
// ---------------------------------------------------------------------------
void processCanService()
{
    CANMessage frameIn;

    // CAN Service → CAN Booster
    if (CanService::receive(frameIn))
    {
        // Forward brut vers CAN Booster
        ACAN_ESP32::can.tryToSend(frameIn);

        // ---------------------------
        // Gestion Discovery 2026
        // ---------------------------
        CANMessage frameOut;

        auto sendMsg = [](CANMessage frameOut) -> bool
        {
            frameOut.id |= idMain;     // ID de la carte Main
            return CanService::send(frameOut);
        };

        const byte priorite = (frameIn.id & 0x1E000000) >> 25;
        const byte cmde     = (frameIn.id & 0x1FE0000) >> 17;
        const uint16_t idExp = frameIn.id & 0xFFFF;

        frameOut.id |= priorite << 25;
        frameOut.ext = true;

        switch (cmde)
        {
        // -------------------------------------------------------
        // 0xB2 : Ping d’un satellite
        // -------------------------------------------------------
        case 0xB2:
            frameOut.len = 1;
            frameOut.id |= 0xB3 << 17;   // Réponse
            frameOut.id |= 0x01 << 16;   // Sous-commande
            frameOut.data[0] = 1;
            sendMsg(frameOut);

            // Enregistrement du satellite si nouveau
            for (byte i = 0; i < NB_SAT; i++)
            {
                if (idExp == sat[i].id())
                    break;

                if (sat[i].id() == NO_ID)
                {
                    sat[i].id(idExp);
                    break;
                }
            }
            break;

        // -------------------------------------------------------
        // 0xB4 : Attribution d’un nouvel ID
        // -------------------------------------------------------
        case 0xB4:
            if (Settings::idNode < 253)
            {
                frameOut.len = 1;
                frameOut.id |= 0xB5 << 17;   // Réponse
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
}
