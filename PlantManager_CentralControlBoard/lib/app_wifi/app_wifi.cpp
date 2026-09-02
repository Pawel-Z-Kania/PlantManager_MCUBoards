#include "app_wifi.h"
#include "config.h"
#include "secrets.h"
#include <ESP8266WiFi.h>
#include <Arduino.h>

// Funkcja pomocnicza do obsługi wyświetlania statusu i blokowania (opcjonalnie z LED)
void app_wifi_init()
{
    Serial.println();
    Serial.print("Laczenie z siecia: ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA); // Ustawiamy ESP wyłącznie na bycie stacją kliencką (wylaczenie AP)
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Czekamy na połączenie. Ponieważ chcemy być pewni połączenia przy restarcie modułu, tu blokujemy.
    // Zabezpieczenie przed nieskończoną pętlą można dodać, gdyby wifi zgasło na długo z punktu widzenia zasilania.
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi polaczone!");
    Serial.print("Adres IP: ");
    Serial.println(WiFi.localIP());
}

void app_wifi_loop()
{
    static unsigned long lastAttempt = 0;
    static uint8_t reconnectFails = 0;

    if (WiFi.status() == WL_CONNECTED)
    {
        reconnectFails = 0;
        return;
    }

    unsigned long now = millis();
    if (now - lastAttempt >= WIFI_RECONNECT_DELAY_MS)
    {
        lastAttempt = now;
        Serial.println("[WiFi] Utracono polaczenie, ponawiam...");
        WiFi.reconnect();
        
        reconnectFails++;
        if (reconnectFails > 15)
        {
            Serial.println("[WiFi] Zbyt wiele nieudanych prob polaczenia, restartowanie urzadzenia...");
            ESP.restart();
        }
    }
}
