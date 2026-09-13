#include "low_power.h"

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/io.h>

// Pusty handler - samo przerwanie WDT budzi CPU z sleep_cpu(), nic więcej nie trzeba robić
ISR(WDT_vect) {}

void logResetReason() {
    // Odczytujemy zanim cokolwiek inne zdąży zmienić MCUSR
    uint8_t reason = MCUSR;
    MCUSR = 0;
    wdt_disable();

    Serial.print(F("[Boot] Powod resetu: "));
    if (reason & (1 << PORF))  Serial.print(F("POWER-ON "));
    if (reason & (1 << EXTRF)) Serial.print(F("EXTERNAL "));
    if (reason & (1 << BORF))  Serial.print(F("BROWN-OUT "));
    if (reason & (1 << WDRF))  Serial.print(F("WATCHDOG "));
    if (reason == 0) Serial.print(F("NIEZNANY"));
    Serial.println();
}

// Uzbraja WDT w trybie tylko-przerwanie (WDIE, bez WDE) i usypia CPU do czasu przerwania
static void wdtSleepOnce(uint8_t wdtPrescalerBits) {
    cli();
    wdt_reset();
    MCUSR &= ~(1 << WDRF);
    // Sekwencja czasowa wymagana przez datasheet do zmiany WDTCSR
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | wdtPrescalerBits;
    sei();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
}

void lowPowerSleepMs(uint32_t ms) {
    // Bez tego ostatnie znaki poprzedniego Serial.print/println (bufor UART jest asynchroniczny)
    // mogłyby zostać ucięte przez wyłączenie zegara peryferiów w SLEEP_MODE_PWR_DOWN
    Serial.flush();

    // Dostępne kroki WDT (czas, bity prescalera WDP3..WDP0), od największego do najmniejszego
    static const struct { uint16_t ms; uint8_t prescalerBits; } steps[] = {
        {8000, (1 << WDP3) | (1 << WDP0)},
        {4000, (1 << WDP3)},
        {2000, (1 << WDP2) | (1 << WDP1) | (1 << WDP0)},
        {1000, (1 << WDP2) | (1 << WDP1)},
        {500,  (1 << WDP2) | (1 << WDP0)},
        {250,  (1 << WDP2)},
        {125,  (1 << WDP1) | (1 << WDP0)},
        {64,   (1 << WDP1)},
        {32,   (1 << WDP0)},
        {16,   0},
    };
    static const uint8_t stepCount = sizeof(steps) / sizeof(steps[0]);

    while (ms >= 16) {
        for (uint8_t i = 0; i < stepCount; i++) {
            if (ms >= steps[i].ms) {
                wdtSleepOnce(steps[i].prescalerBits);
                ms -= steps[i].ms;
                break;
            }
        }
    }

    wdt_disable();
}
