#include "CanBridge.h"
#include "CanService.h"
#include "Globals.h"
#include "Settings.h"

// ---------------------------------------------------------------------------
//  Gestion (commandes B2 / B4) sur le CAN Service
// ---------------------------------------------------------------------------
void processCanService()
{
    CANMessage frameIn;

    // Lire uniquement le CAN Service (MCP2515)
    if (!CanService::receive(frameIn))
        return;

    CANMessage frameOut;

    auto sendMsg = [](CANMessage &msg) -> bool
    {
        msg.id |= idMain;   // ID de la carte Main
        return CanService::send(msg);
    };

    const byte priorite = (frameIn.id & 0x1E000000) >> 25;
    const byte cmde     = (frameIn.id & 0x1FE0000) >> 17;
    const uint16_t idExp = frameIn.id & 0xFFFF;

    frameOut.id  = 0;
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
        frameOut.id |= 0x01 << 16;
        frameOut.data[0] = 1;
        sendMsg(frameOut);
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
