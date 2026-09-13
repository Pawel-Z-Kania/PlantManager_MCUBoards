#pragma once
#include <Arduino.h>
#include <string.h>
#include "config.h"
#include <EEPROM.h>

// Trwały (EEPROM), losowo wygenerowany identyfikator fizycznej płytki - ten sam
// mechanizm co getDeviceUniqueId() w sensor_logic.cpp, tu wydzielony do użytku w testach.
inline uint32_t getBoardId() {
  uint32_t storedId;
  EEPROM.get(EEPROM_ID_ADDR, storedId);

  // Pelny zakres zamiast tylko 0/0xFFFFFFFF - lapie tez czesciowo zapisany (torn write) EEPROM
  if (storedId < BOARD_ID_MIN || storedId > BOARD_ID_MAX) {
    randomSeed(analogRead(A1) + analogRead(A2));
    storedId = random(BOARD_ID_MIN, BOARD_ID_MAX + 1);
    EEPROM.put(EEPROM_ID_ADDR, storedId);
  }

  return storedId;
}
