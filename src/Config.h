#pragma once

#include <Arduino.h>
#include <ACAN2515.h>

// ============================================================================
//  Discovery 2026 - Configuration générale + Profil DCC → CAN Booster
// ============================================================================
//
//  Ce fichier définit :
//   - IDs CAN officiels
//   - timings DCC validés
//   - comportement failsafe
//   - options debug
//   - télémétrie standard
//   - configuration WiFi
//   - configuration CAN Service (MCP2515)
//   - configuration CAN Booster (ACAN_ESP32)
//   - constantes système
//
// ============================================================================

// ---------------------------------------------------------------------------
//  CONFIGURATION GÉNÉRALE
// ---------------------------------------------------------------------------
#define DEBUG
#define debug Serial

#define NO_ID      255
#define NO_PIN     255

#define NB_SAT     30
#define NB_LOCOS   7

// ---------------------------------------------------------------------------
//  CONFIGURATION WIFI
// ---------------------------------------------------------------------------
#define CONFIG 1   // 0 = AP mode, 1 = Client mode

#if CONFIG == 0
    #define WIFI_AP_MODE
    #define WIFI_SSID "digital"
    #define WIFI_PSW  "digital"
#elif CONFIG == 1
    #define WIFI_SSID "Starlink Olivier"
    #define WIFI_PSW  "VF4Ba.C-9M9FWprX"
#endif

#define MDNS_NAME "samain"


// ---------------------------------------------------------------------------
//  CAN IDs (CAN Booster)
// ---------------------------------------------------------------------------
#define DCCB_CAN_ID_DCC_BIT          0x100   // bit DCC (0/1 + phase)
#define DCCB_CAN_ID_CUTOUT           0x101   // cutout local/global
#define DCCB_CAN_ID_TELEMETRY        0x102   // télémétrie booster
#define DCCB_CAN_ID_RAILCOM          0x103   // RailCom (EXSA)

// ---------------------------------------------------------------------------
//  Timings DCC validés
// ---------------------------------------------------------------------------
#define DCCB_TIMING_BIT1_MIN_US      40
#define DCCB_TIMING_BIT1_MAX_US      80
#define DCCB_TIMING_BIT0_MIN_US      90
#define DCCB_TIMING_BIT0_MAX_US      150

// Détection cutout
#define DCCB_TIMING_CUTOUT_START_US  300 // > → début cutout
#define DCCB_TIMING_CUTOUT_END_GAP   80  // marge pour fin cutout

// ---------------------------------------------------------------------------
//  CAN Booster (ACAN_ESP32)
// ---------------------------------------------------------------------------
#define DCCB_CAN_BITRATE             500000   // 500 kbps standard

// ---------------------------------------------------------------------------
//  Failsafe
// ---------------------------------------------------------------------------
#define DCCB_FAILSAFE_TIMEOUT_MS     500  // perte DCC → OFF
#define DCCB_FAILSAFE_COOLDOWN_MS    1000 // réarmement

// ---------------------------------------------------------------------------
//  Télémétrie
// ---------------------------------------------------------------------------
#define DCCB_TELEMETRY_PERIOD_MS     1000 // envoi toutes les 1s

// ---------------------------------------------------------------------------
//  Options debug
// ---------------------------------------------------------------------------
#define DCCB_DEBUG_SERIAL            1  // 0=off, 1=normal, 2=verbeux
#define DCCB_MEASURE_STATS           1
#define DCCB_SCOPE_MODE              0  // 1 = oscilloscope DCC

// ---------------------------------------------------------------------------
//  Taille de la queue d'événements DCC
// ---------------------------------------------------------------------------
#define DCC_EVENT_QUEUE_SIZE         64

// ---------------------------------------------------------------------------
//  CAN Service (MCP2515)
// ---------------------------------------------------------------------------
#define CAN_SERVICE_BITRATE          125000
