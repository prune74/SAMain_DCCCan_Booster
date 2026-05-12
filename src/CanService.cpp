#include "CanService.h"
#include "pins.h"
#include <SPI.h>

static const uint32_t CAN_SERVICE_BITRATE = 250UL * 1000UL;

// Instance unique du driver
ACAN2515 CanService::_can(PIN_MCP2515_CS, SPI, PIN_MCP2515_INT);

bool CanService::begin()
{
    // Initialisation SPI
    SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI);

    ACAN2515Settings settings(16UL * 1000UL * 1000UL, CAN_SERVICE_BITRATE);

    uint32_t errorCode = _can.begin(settings, [] { _can.isr(); });

    return errorCode == 0;
}

bool CanService::receive(CANMessage &msg)
{
    return _can.receive(msg);
}

bool CanService::send(const CANMessage &msg)
{
    return _can.tryToSend(msg);
}

ACAN2515& CanService::driver()
{
    return _can;
}
