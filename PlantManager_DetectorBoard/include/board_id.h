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

  if (storedId == 0xFFFFFFFF || storedId == 0) {
    randomSeed(analogRead(A1) + analogRead(A2));
    storedId = random(100000, 999999);
    EEPROM.put(EEPROM_ID_ADDR, storedId);
  }

  return storedId;
}
