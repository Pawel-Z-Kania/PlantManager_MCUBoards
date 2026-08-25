#include <Arduino.h>
#include "sensor_definitions.h"
#include "config.h"
#include "bsp_pins.h"
#include "self_test.h"

const int SENSOR_POWER = PIN_MOISTURE_POWER;
const int SENSOR_PIN = PIN_MOISTURE_SENSOR;

void setup() {
  pinMode(SENSOR_POWER, OUTPUT);
  digitalWrite(SENSOR_POWER, LOW);
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  selfTestSuiteBegin("test2_sensor");
}

void loop() {
  digitalWrite(SENSOR_POWER, HIGH); // Włącz czujnik
  delay(50);                        // Czas na ustabilizowanie napięcia
  int odczyt = analogRead(SENSOR_PIN);
  digitalWrite(SENSOR_POWER, LOW);  // Wyłącz czujnik

  Serial.print("Wartosc ADC: ");
  Serial.println(odczyt);

  bool valid = (odczyt >= MOISTURE_ADC_MIN) && (odczyt <= MOISTURE_ADC_MAX);
  selfTestReport(valid, "Odczyt czujnika w prawidłowym zakresie (nie utknął na szynie)");
  selfTestSuiteEnd(valid);

  delay(1000);
}