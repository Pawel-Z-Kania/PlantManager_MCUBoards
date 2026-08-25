#include "app_mqtt.h"
#include "bsp_pins.h"
#include "secrets.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Obiekty =========================================================
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi()
{
    delay(10);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        delay(500);
    }

    // BARDZO WAŻNE: HiveMQ używa certyfikatów.
    // setInsecure() pozwala pominąć weryfikację certyfikatu (mniej bezpieczne, ale łatwiejsze na start)
    espClient.setInsecure();

    digitalWrite(LED_PIN, LOW);
}

const unsigned long MQTT_RECONNECT_INTERVAL_MS = 5000;
static unsigned long lastReconnectAttempt = 0;

void reconnectAttempt()
{
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS))
    {
        digitalWrite(LED_PIN, LOW); // Zapal diodę - jest OK
    }
    else
    {
        Serial.print("Blad polaczenia MQTT, rc=");
        Serial.println(client.state());
    }
}

void app_mqtt_init()
{
    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
}

void app_mqtt_loop()
{
    if (!client.connected())
    {
        // Nieblokujący retry - nie zatrzymuje odczytu radia na czas przestoju MQTT
        unsigned long now = millis();
        if (now - lastReconnectAttempt >= MQTT_RECONNECT_INTERVAL_MS)
        {
            lastReconnectAttempt = now;
            reconnectAttempt();
        }
        return;
    }
    client.loop();
}

void app_mqtt_publish_json(uint32_t boardId, const String& jsonData)
{
    // Osobny subtopic per płytka - inaczej płytki nadpisywałyby sobie wzajemnie ten sam temat
    String topic = String(MQTT_TOPIC_BASE) + "/" + String(boardId);
    client.publish(topic.c_str(), jsonData.c_str());
}
