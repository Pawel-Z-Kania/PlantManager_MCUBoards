#include "battery.h"

#include <avr/io.h>

uint16_t readVccMv() {
  // Wybór wewnętrznego 1.1V jako źródła sygnału mierzonego względem AVCC
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else // ATmega328P (Arduino Pro Mini / Uno)
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif

  delay(2); // Czas na ustabilizowanie się napięcia odniesienia

  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC));

  uint8_t low  = ADCL; // ADCL musi być odczytany przed ADCH
  uint8_t high = ADCH;

  long raw = (high << 8) | low;

  // 1125300 = 1.1V * 1023 * 1000 - stała teoretyczna dla 10-bitowego przetwornika
  return (uint16_t)(1125300L / raw);
}
