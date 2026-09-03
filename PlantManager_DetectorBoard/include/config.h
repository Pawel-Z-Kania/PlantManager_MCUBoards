#ifndef CONFIG_H
#define CONFIG_H

#define EEPROM_ID_ADDR 0
#define SERIAL_BAUD 9600

// Szczegolowe logi diagnostyczne (przyczyna resetu, timestampy, wartosci pomiarow) - wylacz w finalnej produkcji
#define DEBUG_LOG 1

// Odstęp między kolejnymi transmisjami czujnika
#define SENSOR_TX_INTERVAL_MS 30000UL

// Ponizej tego napiecia pomijamy nadawanie - zbyt slabe zasilanie powoduje zawieszki/glicze zamiast czystego resetu
#define BATTERY_MIN_MV 2700

// Prawidłowy zakres odczytu ADC - poza tym zakresem czujnik jest odłączony/uszkodzony
#define MOISTURE_ADC_MIN 5
#define MOISTURE_ADC_MAX 1018

#endif
