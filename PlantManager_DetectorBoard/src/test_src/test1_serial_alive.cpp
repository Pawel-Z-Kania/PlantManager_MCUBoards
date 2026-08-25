#include <Arduino.h>
#include "bsp_pins.h"
#include "self_test.h"

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  delay(1000);
  selfTestSuiteBegin("test1_serial_alive");
  selfTestReport(true, "MCU wystartowało i UART działa");
  selfTestSuiteEnd(true);
}

void loop() {
  // Ciągłe "tick" potwierdza stabilną pracę, nie tylko jednorazowy start
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
  Serial.println(F("tick"));
}
