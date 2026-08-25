#pragma once
#include <Arduino.h>
#include <string.h>
#include "config.h"

#if defined(ARDUINO_ARCH_SAM)
#include <DueFlashStorage.h>
static DueFlashStorage dueFlashStorage;

// Due nie ma EEPROM - DueFlashStorage emuluje ten sam trwa\u0142y zapis we flash
inline uint32_t getBoardId() {
  uint32_t storedId;
  memcpy(&storedId, dueFlashStorage.readAddress(EEPROM_ID_ADDR), sizeof(storedId));

  if (storedId == 0xFFFFFFFF || storedId == 0) {
    randomSeed(analogRead(A1) + analogRead(A2));
    storedId = random(100000, 999999);
    dueFlashStorage.write(EEPROM_ID_ADDR, (byte *)&storedId, sizeof(storedId));
  }

  return storedId;
}

#else
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
#endif
