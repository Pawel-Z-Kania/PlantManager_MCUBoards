#include "hal_radio.h"
#include "bsp_pins.h"
#include "radio_config.h"
#include <SPI.h>
#include <RF24.h>

// Obiekt radia dostępny tylko w tym pliku (static)
static RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

// Adres rury komunikacyjnej - musi być identyczny z nadajnikiem
const byte address[6] = NRF_ADDRESS;

void hal_radio_init() {
    // Inicjalizacja magistrali SPI i radia - ponawiaj zamiast zawieszać się na stałe
    uint8_t attempts = 0;
    while (!radio.begin()) {
        Serial.println(F("BŁĄD: Nie znaleziono modułu nRF24L01!"));
        delay(500);
        if (++attempts >= 10) {
            ESP.restart(); // jawny, logowany reset zamiast cichego zawieszenia
        }
    }

    // Ustawienie mocy nadawania - LOW wystarczy na odległość kilku metrów
    // i minimalizuje zakłócenia przy zasilaniu z USB
    radio.setPALevel(NRF_PA_LEVEL);
    radio.setDataRate(NRF_DATA_RATE);
    radio.setChannel(NRF_CHANNEL);

    // Otwarcie rury do odczytu (indeks 1) - dla ACK jest 1
    radio.openReadingPipe(1, address);

    // Przełączenie w tryb odbiornika
    radio.startListening();
}

bool hal_radio_read(PlantData &data) {
    if (radio.available()) {
        radio.read(&data, sizeof(data));
        return true; 
    }
    
    return false; 
}