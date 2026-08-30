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

void app_http_publish(const PlantData &data)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("[HTTP] Brak WiFi, zrzucam pakiet.");
        return;
    }

    HTTPClient http;
    String url = String(VERCEL_URL) + HTTP_ENDPOINT_PATH;
    http.begin(wifiClient, url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", HTTP_AUTH_TOKEN );

    String body = "{\"board_id\":" + String(data.id) + ",\"value\":" + String(data.moisture) + "}";
    int code = http.POST(body);

    if (code > 0)
    {
        Serial.print("[HTTP] Kod odpowiedzi: ");
        Serial.println(code);
        if (code != 200)
            Serial.println("[HTTP] Tresc odpowiedzi: " + http.getString());
    }
    else
    {
        Serial.println("[HTTP] Blad sieci: " + http.errorToString(code));
    }

    http.end();
}
