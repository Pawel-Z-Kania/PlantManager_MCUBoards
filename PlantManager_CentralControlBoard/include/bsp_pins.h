#pragma once
#include <Arduino.h>

// Piny dla nRF24L01 na NodeMCU
#define NRF_CE_PIN  4  // D2
#define NRF_CSN_PIN 15 // D8
#define LED_PIN     2  // D4 

// Sprzętowe piny SPI na ESP8266 (NodeMCU) - stałe, wynikają z układu scalonego
#define SPI_SCK_PIN  14 // D5
#define SPI_MOSI_PIN 13 // D7
#define SPI_MISO_PIN 12 // D6