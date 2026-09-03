#pragma once

// Parametry strojenia - bezpieczne do commitu, credentials tylko w secrets.h
#define DEBUG_LOG 1
#define HTTP_ENDPOINT_PATH      "/api/moisture"
#define HTTP_RETRY_INTERVAL_MS  10000UL
#define WIFI_RECONNECT_DELAY_MS  5000UL
#define SERIAL_BAUD_RATE        115200
// Brak odebranego pakietu przez ten czas = radio uznane za zawieszone, wymuszamy reinicjalizacje
#define RADIO_WATCHDOG_TIMEOUT_MS 90000UL
