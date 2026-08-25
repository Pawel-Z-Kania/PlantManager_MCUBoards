
#include <Arduino.h>
#include "bsp_pins.h"
#include "hal_radio.h"
#include "app_mqtt.h"

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    hal_radio_init();
    app_mqtt_init();

    digitalWrite(LED_PIN, LOW);
    Serial.println("System Monitoringu (ESP8266) Gotowy!");
}

void loop()
{
    PlantData receivedData;

    app_mqtt_loop();

    if (hal_radio_read(receivedData))
    {
        String jsonPayload = "{\"id\":" + String(receivedData.id) +
                             ",\"val\":" + String(receivedData.moisture) + "}";

        // Wysyłamy każdą odebraną paczkę do HiveMQ
        app_mqtt_publish_json(receivedData.id, jsonPayload);
    }
}