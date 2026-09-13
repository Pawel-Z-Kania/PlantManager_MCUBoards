
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "bsp_pins.h"
#include "hal_radio.h"
#include "app_wifi.h"
#include "app_http.h"
#include "config.h"

bool transmissionError = false;

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    hal_radio_init();
    app_wifi_init();
    app_http_init();

    digitalWrite(LED_PIN, LOW); // Na NodeMCU stan LOW włacza obudowaną diodę
    Serial.println("System Monitoringu (ESP8266) Gotowy!");
}

void loop()
{
    PlantData receivedData;
    static unsigned long lastRxMillis = millis();
    // Ostatni PRZYJETY pakiet - do wykrywania retransmisji nRF24 po zgubionym ACK (ta sama tresc, krotko po sobie)
    static uint32_t lastAcceptedId = 0;
    static int16_t lastAcceptedMoisture = 0;
    static uint16_t lastAcceptedBatteryMv = 0;
    static unsigned long lastAcceptedMillis = 0;

    app_wifi_loop();

    if (hal_radio_read(receivedData))
    {
        lastRxMillis = millis(); // radio zylo niezaleznie od tresci pakietu

        if (receivedData.id < BOARD_ID_MIN || receivedData.id > BOARD_ID_MAX)
        {
            Serial.println(F("[ RX ] odrzucono pakiet (id poza zakresem)"));
        }
        else if (receivedData.id == lastAcceptedId &&
                 receivedData.moisture == lastAcceptedMoisture &&
                 receivedData.batteryMv == lastAcceptedBatteryMv &&
                 millis() - lastAcceptedMillis < DUPLICATE_PACKET_WINDOW_MS)
        {
            Serial.println(F("[ RX ] odrzucono duplikat (retransmisja po zgubionym ACK)"));
        }
        else
        {
            lastAcceptedId = receivedData.id;
            lastAcceptedMoisture = receivedData.moisture;
            lastAcceptedBatteryMv = receivedData.batteryMv;
            lastAcceptedMillis = millis();

#if DEBUG_LOG
            Serial.print(F("[ RX ] id="));
            Serial.print(receivedData.id);
            Serial.print(F(" wilgotnosc="));
            Serial.print(receivedData.moisture);
            Serial.print(F(" bateria="));
            Serial.print(receivedData.batteryMv);
            Serial.print(F("mV t="));
            Serial.print(millis());
            Serial.println(F("ms"));
#endif

            bool success = app_http_publish(receivedData);
            transmissionError = !success;
        }
    }

    if (millis() - lastRxMillis >= RADIO_WATCHDOG_TIMEOUT_MS)
    {
        Serial.print(F("[Radio] Brak pakietow od "));
        Serial.print(RADIO_WATCHDOG_TIMEOUT_MS / 1000);
        Serial.println(F("s, reinicjalizacja..."));
        hal_radio_init();
        lastRxMillis = millis();
    }

    // Sygnalizacja błędów - jeśli nie ma wifi albo wysłanie paczki się nie udało, migaj diodą
    static unsigned long lastBlinkTime = 0;
    if (WiFi.status() != WL_CONNECTED || transmissionError)
    {
        if (millis() - lastBlinkTime >= 500)
        {
            lastBlinkTime = millis();
            digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        }
    }
    else
    {
        // W przeciwnym razie swieci się cały czas
        digitalWrite(LED_PIN, LOW);
    }
}   