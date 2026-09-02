#pragma once
#include <Arduino.h>

// Zwracane gdy pomiar VCC nie jest dostępny na danej platformie
#define BATTERY_MV_NA 0xFFFF

// Odczyt napięcia zasilania (VCC) przez wewnętrzne źródło odniesienia 1.1V - bez użycia pinu ADC
uint16_t readVccMv();
