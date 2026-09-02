#include "app_http.h"
#include "config.h"
#include "secrets.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

static WiFiClientSecure wifiClient;

void app_http_init()
{
    wifiClient.setInsecure();
    Serial.println("[HTTP] Zainicjowano.");
}

bool app_http_publish(const PlantData &data)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("[HTTP] Brak WiFi, zrzucam pakiet.");
        return false;
    }

    HTTPClient http;
    String url = String(VERCEL_URL) + HTTP_ENDPOINT_PATH;
    http.begin(wifiClient, url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", HTTP_AUTH_TOKEN );

    String body = "{\"board_id\":" + String(data.id) + ",\"value\":" + String(data.moisture) + ",\"battery_mv\":" + String(data.batteryMv) + "}";
    int code = http.POST(body);

    bool success = false;
    if (code > 0)
    {
        Serial.print("[HTTP] Kod odpowiedzi: ");
        Serial.println(code);
        if (code == 200) {
            success = true;
        } else {
            Serial.println("[HTTP] Tresc odpowiedzi: " + http.getString());
        }
    }
    else
    {
        Serial.println("[HTTP] Blad sieci: " + http.errorToString(code));
    }

    http.end();
    return success;
}
