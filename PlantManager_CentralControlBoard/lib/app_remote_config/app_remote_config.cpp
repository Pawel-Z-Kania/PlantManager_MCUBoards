#include "app_remote_config.h"

#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include "config.h"
#include "hal_radio.h"
#include "secrets.h"

static WiFiClientSecure configWifiClient;
static unsigned long lastConfigAttemptMillis = 0;
static bool hasAttemptedConfigFetch = false;
static uint32_t activeConfigVersion = 1;
static uint16_t activeIntervalSeconds = DEFAULT_SENSOR_SAMPLE_INTERVAL_SEC;

struct __attribute__((packed)) StoredRemoteConfig {
    uint32_t magic;
    uint32_t configVersion;
    uint16_t intervalSeconds;
    uint32_t checksum;
};

static uint32_t configChecksum(uint32_t configVersion, uint16_t intervalSeconds)
{
    return CONFIG_STORAGE_MAGIC ^ configVersion ^ intervalSeconds ^ 0xA5A55A5AUL;
}

static bool isValidConfig(uint32_t intervalSeconds, uint32_t configVersion)
{
    return configVersion > 0 &&
           intervalSeconds >= SENSOR_SAMPLE_INTERVAL_MIN_SEC &&
           intervalSeconds <= SENSOR_SAMPLE_INTERVAL_MAX_SEC;
}

static void saveConfig(uint32_t configVersion, uint16_t intervalSeconds)
{
    const StoredRemoteConfig storedConfig = {
        CONFIG_STORAGE_MAGIC,
        configVersion,
        intervalSeconds,
        configChecksum(configVersion, intervalSeconds),
    };
    EEPROM.put(CONFIG_EEPROM_ADDR, storedConfig);
    EEPROM.commit();
}

static void loadStoredConfig()
{
    StoredRemoteConfig storedConfig = {};
    EEPROM.get(CONFIG_EEPROM_ADDR, storedConfig);

    if (storedConfig.magic == CONFIG_STORAGE_MAGIC &&
        isValidConfig(storedConfig.intervalSeconds, storedConfig.configVersion) &&
        storedConfig.checksum == configChecksum(storedConfig.configVersion, storedConfig.intervalSeconds))
    {
        activeConfigVersion = storedConfig.configVersion;
        activeIntervalSeconds = storedConfig.intervalSeconds;
        hal_radio_set_config(storedConfig.configVersion, storedConfig.intervalSeconds);
    }
}

static bool fetchConfig()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return false;
    }

    HTTPClient http;
    const String url = String(VERCEL_URL) + DEVICE_CONFIG_ENDPOINT_PATH;
    http.begin(configWifiClient, url);
    http.addHeader("x-api-key", HTTP_AUTH_TOKEN);

    const int code = http.GET();
    if (code != HTTP_CODE_OK)
    {
        Serial.print(F("[Config] Blad HTTP: "));
        Serial.println(code);
        http.end();
        return false;
    }

    JsonDocument document;
    const DeserializationError error = deserializeJson(document, http.getStream());
    http.end();
    if (error)
    {
        Serial.print(F("[Config] Nieprawidlowy JSON: "));
        Serial.println(error.c_str());
        return false;
    }

    const uint32_t intervalSeconds = document["sensor_sample_interval_sec"] | 0;
    const uint32_t configVersion = document["config_version"] | 0;
    if (!isValidConfig(intervalSeconds, configVersion))
    {
        Serial.println(F("[Config] Odrzucono konfiguracje poza zakresem."));
        return false;
    }

    const uint16_t newIntervalSeconds = static_cast<uint16_t>(intervalSeconds);
    if (configVersion != activeConfigVersion || newIntervalSeconds != activeIntervalSeconds)
    {
        activeConfigVersion = configVersion;
        activeIntervalSeconds = newIntervalSeconds;
        hal_radio_set_config(configVersion, newIntervalSeconds);
        saveConfig(configVersion, newIntervalSeconds);
    }
#if DEBUG_LOG
    Serial.print(F("[Config] Wersja="));
    Serial.print(configVersion);
    Serial.print(F(" interwal="));
    Serial.print(intervalSeconds);
    Serial.println(F("s"));
#endif
    return true;
}

void app_remote_config_init()
{
    configWifiClient.setInsecure();
    EEPROM.begin(CONFIG_EEPROM_SIZE);
    hal_radio_set_config(1, DEFAULT_SENSOR_SAMPLE_INTERVAL_SEC);
    loadStoredConfig();
}

void app_remote_config_loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    const unsigned long now = millis();
    if (hasAttemptedConfigFetch && now - lastConfigAttemptMillis < CONFIG_SYNC_INTERVAL_MS)
    {
        return;
    }

    lastConfigAttemptMillis = now;
    hasAttemptedConfigFetch = true;
    fetchConfig();
}

unsigned long app_remote_config_get_radio_watchdog_timeout_ms()
{
    const unsigned long intervalTimeoutMs =
        static_cast<unsigned long>(activeIntervalSeconds) * 1000UL * RADIO_WATCHDOG_MISSED_REPORTS +
        RADIO_WATCHDOG_MARGIN_MS;
    return intervalTimeoutMs > RADIO_WATCHDOG_MIN_TIMEOUT_MS
               ? intervalTimeoutMs
               : RADIO_WATCHDOG_MIN_TIMEOUT_MS;
}