#include "sensor_definitions.h"
#include <RF24.h>
#include "config.h"
#include "bsp_pins.h"
#include "board_id.h"
#include "config_protocol.h"
#include "radio_config.h"

// DEKLARACJE ZMIENNYCH  =========================================

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

struct __attribute__((packed)) StoredSensorConfig {
    uint32_t magic;
    uint32_t configVersion;
    uint16_t intervalSeconds;
    uint32_t checksum;
};

static uint32_t activeConfigVersion = 0;
static uint16_t activeIntervalSeconds = SENSOR_TX_INTERVAL_MS / 1000UL;

// Adres kanału do nRF24
const byte address[6] = NRF_ADDRESS;

static uint32_t configChecksum(uint32_t configVersion, uint16_t intervalSeconds)
{
    return SENSOR_CONFIG_STORAGE_MAGIC ^ configVersion ^ intervalSeconds ^ 0xA5A55A5AUL;
}

static bool isValidInterval(uint16_t intervalSeconds)
{
    return intervalSeconds >= SENSOR_SAMPLE_INTERVAL_MIN_SEC &&
           intervalSeconds <= SENSOR_SAMPLE_INTERVAL_MAX_SEC;
}

static void loadStoredSensorConfig()
{
    StoredSensorConfig storedConfig;
    EEPROM.get(EEPROM_CONFIG_ADDR, storedConfig);

    if (storedConfig.magic == SENSOR_CONFIG_STORAGE_MAGIC &&
        storedConfig.configVersion > 0 &&
        isValidInterval(storedConfig.intervalSeconds) &&
        storedConfig.checksum == configChecksum(storedConfig.configVersion, storedConfig.intervalSeconds))
    {
        activeConfigVersion = storedConfig.configVersion;
        activeIntervalSeconds = storedConfig.intervalSeconds;
    }
}

static void applyConfigPayload(const RadioConfigPacket &packet, uint32_t boardId)
{
    if (packet.magic != CONFIG_PACKET_MAGIC ||
        packet.protocolVersion != CONFIG_PACKET_PROTOCOL_VERSION ||
        (packet.targetBoardId != CONFIG_BROADCAST_BOARD_ID && packet.targetBoardId != boardId) ||
        packet.configVersion <= activeConfigVersion ||
        !isValidInterval(packet.sensorSampleIntervalSec))
    {
        return;
    }

    activeConfigVersion = packet.configVersion;
    activeIntervalSeconds = packet.sensorSampleIntervalSec;
    const StoredSensorConfig storedConfig = {
        SENSOR_CONFIG_STORAGE_MAGIC,
        activeConfigVersion,
        activeIntervalSeconds,
        configChecksum(activeConfigVersion, activeIntervalSeconds),
    };
    EEPROM.put(EEPROM_CONFIG_ADDR, storedConfig);

#if DEBUG_LOG
    Serial.print(F("[Config] Wersja="));
    Serial.print(activeConfigVersion);
    Serial.print(F(" interwal="));
    Serial.print(activeIntervalSeconds);
    Serial.println(F("s"));
#endif
}

// METODY GLOBALNE ===============================================
void setupHardware() {
    Serial.begin(SERIAL_BAUD);
    loadStoredSensorConfig();
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
    radio.enableDynamicPayloads();
    radio.enableAckPayload();

    // Otwieramy kanał do nadawania (zapisywania)
    radio.openWritingPipe(address);

    // Nadajnik nie nasłuchuje, tylko nadaje
    radio.stopListening();
};

void transmitSensorData(const plantData& dataObject)
{
    // Wybudź radio ze stanu power-down (patrz powerDown() poniżej) przed nadawaniem
    radio.powerUp();
    delay(5);

    // Wysyłamy strukturę binarnie (mieści się w 32 bajtach)
    bool sukces = radio.write(&dataObject, sizeof(dataObject));

    if (sukces && radio.isAckPayloadAvailable())
    {
        const uint8_t payloadSize = radio.getDynamicPayloadSize();
        if (payloadSize == sizeof(RadioConfigPacket))
        {
            RadioConfigPacket packet;
            radio.read(&packet, sizeof(packet));
            applyConfigPayload(packet, dataObject.id);
        }
        else
        {
            radio.flush_rx();
        }
    }

    // Zejdź do najniższego poboru prądu do następnej transmisji
    radio.powerDown();

#if DEBUG_LOG
    Serial.print(F("[TX] t="));
    Serial.print(millis());
    Serial.print(F("ms id="));
    Serial.print(dataObject.id);
    Serial.print(F(" wilgotnosc="));
    Serial.print(dataObject.moisture);
    Serial.print(F(" bateria="));
    Serial.print(dataObject.batteryMv);
    Serial.print(F("mV status="));
    Serial.println(sukces ? F("OK") : F("BLAD"));
#endif

    if (!sukces)
    {
        Serial.println("Blad wysylania - brak odbiornika?");
    }
}

unsigned long getSensorTxIntervalMs()
{
    return static_cast<unsigned long>(activeIntervalSeconds) * 1000UL;
}
