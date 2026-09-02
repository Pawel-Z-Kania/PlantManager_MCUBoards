
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "bsp_pins.h"
#include "hal_radio.h"
#include "app_wifi.h"
#include "app_http.h"

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

    app_wifi_loop();

    if (hal_radio_read(receivedData))
    {
        bool success = app_http_publish(receivedData);
        transmissionError = !success;
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