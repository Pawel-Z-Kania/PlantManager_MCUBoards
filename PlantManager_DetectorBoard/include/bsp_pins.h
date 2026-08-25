#ifndef BSP_PINS_H
#define BSP_PINS_H

#include <Arduino.h>

// Piny nRF24L01 - wspólne dla płytek AVR (Pro Mini / Uno)
#define NRF_CE_PIN   8
#define NRF_CSN_PIN  10
#define SPI_SCK_PIN  13 // pokrywa się z wbudowaną diodą LED na Uno/Pro Mini
#define SPI_MOSI_PIN 11
#define SPI_MISO_PIN 12

// Wbudowana dioda diagnostyczna (ten sam pin co SCK)
#define LED_PIN 13

// Czujnik wilgotności
#define PIN_MOISTURE_SENSOR A0
#define PIN_MOISTURE_POWER 7

#endif
