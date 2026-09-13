#ifndef CONFIG_H
#define CONFIG_H

#define EEPROM_ID_ADDR 0
#define EEPROM_CONFIG_ADDR 8
#define SERIAL_BAUD 9600

// Jedyny prawidlowy zakres ID plytki (patrz board_id.h) - poza nim EEPROM uznajemy za pusty/uszkodzony
#define BOARD_ID_MIN 100000UL
#define BOARD_ID_MAX 999999UL

// Szczegolowe logi diagnostyczne (przyczyna resetu, timestampy, wartosci pomiarow) - wylacz w finalnej produkcji
#define DEBUG_LOG 1

// Odstęp między kolejnymi transmisjami czujnika
#define SENSOR_TX_INTERVAL_MS 30000UL
#define SENSOR_SAMPLE_INTERVAL_MIN_SEC 1UL
#define SENSOR_SAMPLE_INTERVAL_MAX_SEC 3600UL
#define SENSOR_CONFIG_STORAGE_MAGIC 0x504D5343UL

// Ponizej tego napiecia pomijamy nadawanie - zbyt slabe zasilanie powoduje zawieszki/glicze zamiast czystego resetu
#define BATTERY_MIN_MV 2700

// Prawidłowy zakres odczytu ADC - poza tym zakresem czujnik jest odłączony/uszkodzony
#define MOISTURE_ADC_MIN 5
#define MOISTURE_ADC_MAX 1018

#endif
