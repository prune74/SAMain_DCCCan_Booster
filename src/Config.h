/*


*/

#ifndef __CONFIG__
#define __CONFIG__

#include <Arduino.h>
#include <ACAN_ESP32.h>

#define DEBUG

#define NO_ID      255       
#define NO_PIN     255

#define NB_SAT     30
#define NB_LOCOS   7

/* ----- Debug   -------------------*/
#define DEBUG
#define debug Serial


/* ----- Wifi --------------------*/

#define CONFIG 1 // Selection du mode

#if CONFIG == 0 // WiFi en mode point d’accès
#define WIFI_AP_MODE
#define WIFI_SSID "digital"
#define WIFI_PSW "digital" // Password facultatif

#elif CONFIG == 1
#define WIFI_SSID "Starlink Olivier"
#define WIFI_PSW "VF4Ba.C-9M9FWprX"



#endif

#endif
