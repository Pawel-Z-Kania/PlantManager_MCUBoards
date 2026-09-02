#pragma once
#include <Arduino.h>

// Usypia MCU (power-down) na zadany czas zamiast aktywnie czekać jak delay() - patrz low_power.cpp
void lowPowerSleepMs(uint32_t ms);
