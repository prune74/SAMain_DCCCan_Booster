#pragma once

#include <Arduino.h>
#include "Satellite.h"
#include "Config.h"

// ID de la carte Main (Discovery 2026)
extern uint16_t idMain;

// Tableau des satellites connus
extern Satellite sat[NB_SAT];
