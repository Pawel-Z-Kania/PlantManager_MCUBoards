#include "sensor_definitions.h"
#include <RF24.h>
#include "config.h"
#include "bsp_pins.h"
#include "radio_config.h"

// DEKLARACJE ZMIENNYCH  =========================================

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

// Adres kanału do nRF24
const byte address[6] = NRF_ADDRESS;

// METODY GLOBALNE ===============================================
void setupHardware() {
    Serial.begin(SERIAL_BAUD);
    pinMode(PIN_MOISTURE_SENSOR, INPUT);
    pinMode(PIN_MOISTURE_POWER, OUTPUT);
    digitalWrite(PIN_MOISTURE_POWER, LOW); // Wyłącz zasilanie na starcie
}

void setupRadio()
{
    if (!radio.begin())
    {
        Serial.println("Nie wykryto radia nRF24! Sprawdz kable.");
        while (1);
    }

    // Ustawienie poziomu mocy (na biurku wystarczy LOW, oszczedza prad)
    radio.setPALevel(NRF_PA_LEVEL);
    radio.setDataRate(NRF_DATA_RATE);
    radio.setChannel(NRF_CHANNEL);

    // Otwieramy kanał do nadawania (zapisywania)
    radio.openWritingPipe(address);

    // Nadajnik nie nasłuchuje, tylko nadaje
    radio.stopListening();
};

void app_mqtt_publish_json(const plantData& dataObject)
{
    // Wybudź radio ze stanu power-down (patrz powerDown() poniżej) przed nadawaniem
    radio.powerUp();
    delay(5);

    // Wysyłamy strukturę binarnie (mieści się w 32 bajtach)
    bool sukces = radio.write(&dataObject, sizeof(dataObject));

    // Zejdź do najniższego poboru prądu do następnej transmisji
    radio.powerDown();

    if (!sukces)
    {
        Serial.println("Blad wysylania - brak odbiornika?");
    }
}
