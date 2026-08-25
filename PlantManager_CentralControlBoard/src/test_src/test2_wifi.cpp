#include <Arduino.h>
#include "app_mqtt.h"
#include "self_test.h"

// Wykorzystuje istniejące dane logowania z app_mqtt.cpp, żeby ich nie duplikować
void setup() {
  Serial.begin(115200);
  delay(1000);
  selfTestSuiteBegin("test2_wifi");

  app_mqtt_init(); // blokuje do połączenia WiFi, potem konfiguruje MQTT
  selfTestReport(true, "WiFi połączone");

  selfTestSuiteEnd(true);
}

void loop() {
  app_mqtt_loop();
}
