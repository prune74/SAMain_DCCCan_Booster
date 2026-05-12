#pragma once
#include <Arduino.h>

// Activation du mode CAN Monitor (CLI)
extern volatile bool canMonitorEnabled;

// Filtre CAN Monitor (ID 11 bits ou -1 = off)
extern volatile int32_t canMonitorFilter;
