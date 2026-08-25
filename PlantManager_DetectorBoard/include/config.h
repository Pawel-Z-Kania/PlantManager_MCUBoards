#ifndef CONFIG_H
#define CONFIG_H

#define EEPROM_ID_ADDR 0
#define SERIAL_BAUD 9600

// Odstęp między kolejnymi transmisjami czujnika
#define SENSOR_TX_INTERVAL_MS 30000UL

// Prawidłowy zakres odczytu ADC - poza tym zakresem czujnik jest odłączony/uszkodzony
#define MOISTURE_ADC_MIN 5
#define MOISTURE_ADC_MAX 1018

#endif