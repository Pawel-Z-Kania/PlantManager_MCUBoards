#include "hal_radio.h"
#include "bsp_pins.h"
#include "config_protocol.h"
#include "radio_config.h"
#include <SPI.h>
#include <RF24.h>

// Obiekt radia dostępny tylko w tym pliku (static)
static RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

// Adres rury komunikacyjnej - musi być identyczny z nadajnikiem
const byte address[6] = NRF_ADDRESS;
static RadioConfigPacket configPacket = {
    CONFIG_PACKET_MAGIC,
    CONFIG_PACKET_PROTOCOL_VERSION,
    1,
    CONFIG_BROADCAST_BOARD_ID,
    30,
};

static void queueConfigPayload()
{
    radio.writeAckPayload(1, &configPacket, sizeof(configPacket));
}

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
    radio.enableDynamicPayloads();
    radio.enableAckPayload();

    // Otwarcie rury do odczytu (indeks 1) - dla ACK jest 1
    radio.openReadingPipe(1, address);

    // Przełączenie w tryb odbiornika
    radio.startListening();
    radio.flush_rx(); // czysty start/reinicjalizacja bez zalegajacych pakietow w FIFO
    radio.flush_tx();
    queueConfigPayload();
    queueConfigPayload();
    queueConfigPayload();
}

bool hal_radio_read(PlantData &data) {
    if (radio.available()) {
        radio.read(&data, sizeof(data));
        queueConfigPayload();
        return true; 
    }
    
    return false; 
}

void hal_radio_set_config(uint32_t configVersion, uint16_t intervalSeconds)
{
    configPacket.configVersion = configVersion;
    configPacket.sensorSampleIntervalSec = intervalSeconds;
    radio.flush_tx();
    queueConfigPayload();
    queueConfigPayload();
    queueConfigPayload();
}