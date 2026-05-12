#pragma once
#include <Arduino.h>

//
// ===============================
//  DCC_CAN-Booster (DCC + CAN natif)
// ===============================
//

// Entrée DCC logique (sortie XOR SN74LVC1G86)
static const gpio_num_t PIN_DCC_IN = GPIO_NUM_27;

// CAN natif ESP32 (ACAN_ESP32)
// IMPORTANT : GPIO 21/22 = pins stables et recommandées
static const gpio_num_t PIN_CAN_TX = GPIO_NUM_21;
static const gpio_num_t PIN_CAN_RX = GPIO_NUM_22;

// LED debug Booster
static const gpio_num_t PIN_LED = GPIO_NUM_2;


//
// ===============================
//  SAMain (MCP2515 + SPI)
// ===============================
//

// MCP2515 CAN Service
static const gpio_num_t PIN_MCP2515_CS  = GPIO_NUM_5;   // Chip Select
static const gpio_num_t PIN_MCP2515_INT = GPIO_NUM_4;   // Interrupt

// SPI bus pour MCP2515
static const gpio_num_t PIN_SPI_SCK  = GPIO_NUM_18;
static const gpio_num_t PIN_SPI_MISO = GPIO_NUM_19;
static const gpio_num_t PIN_SPI_MOSI = GPIO_NUM_23;


//
// ===============================
//  Réserves / Documentation
// ===============================
//
// GPIO 0  : Boot mode (éviter)
// GPIO 1  : TX0 (UART0)
// GPIO 3  : RX0 (UART0)
// GPIO 12 : Boot strapping (éviter)
// GPIO 34-39 : entrées uniquement
//
// GPIO libres pour extensions éventuelles :
// 13, 14, 15, 16, 17, 25, 26, 32, 33
//
