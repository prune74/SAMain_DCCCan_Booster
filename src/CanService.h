#pragma once
#include <Arduino.h>
#include <ACAN2515.h>

class CanService
{
public:
    static bool begin();
    static bool receive(CANMessage &msg);
    static bool send(const CANMessage &msg);

    static ACAN2515& driver();   // accès direct si nécessaire

private:
    static ACAN2515 _can;
};
