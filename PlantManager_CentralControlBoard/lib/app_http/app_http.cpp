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
    // Domyslne bufory BearSSL (16KB RX + 16KB TX) nie miesza sie w wolnym heapie ESP8266 - zmniejszamy je
    wifiClient.setBufferSizes(TLS_RX_BUFFER_SIZE, TLS_TX_BUFFER_SIZE);
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
    http.setTimeout(HTTP_TIMEOUT_MS);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("x-api-key", HTTP_AUTH_TOKEN );

    String body = "{\"board_id\":" + String(data.id) + ",\"value\":" + String(data.moisture) + ",\"battery_mv\":" + String(data.batteryMv) + "}";
    int code = http.POST(body);

    bool success = false;
    if (code > 0)
    {
        if (code >= 200 && code < 300) {
            success = true;
        } else {
            Serial.println("[HTTP] Tresc odpowiedzi: " + http.getString());
        }
    }
    else
    {
        Serial.println("[HTTP] Blad sieci: " + http.errorToString(code));
#if DEBUG_LOG
        Serial.print(F("[HTTP] Wolny heap="));
        Serial.print(ESP.getFreeHeap());
        Serial.println(F("B"));
#endif
    }

#if DEBUG_LOG
    Serial.print(F("[HTTP] id="));
    Serial.print(data.id);
    Serial.print(F(" wilgotnosc="));
    Serial.print(data.moisture);
    Serial.print(F(" bateria="));
    Serial.print(data.batteryMv);
    Serial.print(F("mV kod="));
    Serial.print(code);
    Serial.print(F(" status="));
    Serial.println(success ? F("OK") : F("BLAD"));
#endif

    http.end();
    return success;
}
