
#include <Arduino.h>
#include "bsp_pins.h"
#include "hal_radio.h"
#include "app_wifi.h"
#include "app_http.h"

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    hal_radio_init();
    app_wifi_init();
    app_http_init();

    digitalWrite(LED_PIN, LOW);
    Serial.println("System Monitoringu (ESP8266) Gotowy!");
}

void loop()
{
    PlantData receivedData;

    app_wifi_loop();

    if (hal_radio_read(receivedData))
    {
        app_http_publish(receivedData);
    }
}   