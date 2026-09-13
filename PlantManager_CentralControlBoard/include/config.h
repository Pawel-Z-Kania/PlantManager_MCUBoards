#pragma once

// Parametry strojenia - bezpieczne do commitu, credentials tylko w secrets.h
#define DEBUG_LOG 1
#define HTTP_ENDPOINT_PATH      "/api/moisture"
#define DEVICE_CONFIG_ENDPOINT_PATH "/api/device-config"
#define HTTP_RETRY_INTERVAL_MS  10000UL
// Vercel cold start moze potrzebowac wiecej czasu niz domyslny timeout biblioteki (5s)
#define HTTP_TIMEOUT_MS 15000UL
// Domyslne bufory BearSSL (16KB RX+TX) nie miesza sie w wolnym heapie ESP8266 przy tylko ~20KB wolnego
#define TLS_RX_BUFFER_SIZE 1024
#define TLS_TX_BUFFER_SIZE 1024
#define WIFI_RECONNECT_DELAY_MS  5000UL
#define SERIAL_BAUD_RATE        115200
#define CONFIG_SYNC_INTERVAL_MS 3600000UL
#define SENSOR_SAMPLE_INTERVAL_MIN_SEC 1UL
#define SENSOR_SAMPLE_INTERVAL_MAX_SEC 3600UL
#define DEFAULT_SENSOR_SAMPLE_INTERVAL_SEC 30UL
#define RADIO_WATCHDOG_MIN_TIMEOUT_MS 90000UL
#define RADIO_WATCHDOG_MISSED_REPORTS 3UL
#define RADIO_WATCHDOG_MARGIN_MS 10000UL
#define CONFIG_STORAGE_MAGIC 0x504D5343UL
#define CONFIG_EEPROM_SIZE 64
#define CONFIG_EEPROM_ADDR 0
// Ten sam pakiet (id+wartosci) odebrany ponownie w tym oknie = retransmisja po zgubionym ACK, nie nowy odczyt
#define DUPLICATE_PACKET_WINDOW_MS 10000UL
// Jedyny prawidlowy zakres ID plytki (patrz Detector's board_id.h) - poza nim pakiet jest odrzucany
#define BOARD_ID_MIN 100000UL
#define BOARD_ID_MAX 999999UL
