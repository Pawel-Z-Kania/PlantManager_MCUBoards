#include <Arduino.h>
#include "bsp_pins.h"
#include "self_test.h"

void setup() {
  Serial.begin(9600);
  delay(1000);
  selfTestSuiteBegin("test0_selftest_pins");

  bool allPassed = true;

  // Wybierz CSN=HIGH, zeby nRF24 nie sterowal aktywnie linia MISO podczas testu
  pinMode(NRF_CSN_PIN, OUTPUT);
  digitalWrite(NRF_CSN_PIN, HIGH);

  pinMode(SPI_SCK_PIN, OUTPUT);
  digitalWrite(SPI_SCK_PIN, HIGH);
  delay(10);
  bool sckOk = digitalRead(SPI_SCK_PIN);
  selfTestReport(sckOk, "SCK (13) toggluje na HIGH");
  allPassed &= sckOk;

  pinMode(SPI_MOSI_PIN, OUTPUT);
  digitalWrite(SPI_MOSI_PIN, HIGH);
  delay(10);
  bool mosiOk = digitalRead(SPI_MOSI_PIN);
  selfTestReport(mosiOk, "MOSI (11) toggluje na HIGH");
  allPassed &= mosiOk;

  // MISO to wejście - pull-up potwierdza tylko brak zwarcia do GND, nie pełną ciągłość
  pinMode(SPI_MISO_PIN, INPUT_PULLUP);
  delay(10);
  bool misoOk = digitalRead(SPI_MISO_PIN);
  selfTestReport(misoOk, "MISO (12) podciągnięty do HIGH (brak zwarcia do GND)");
  allPassed &= misoOk;

  selfTestSuiteEnd(allPassed);
}

void loop() {
  // Test jednorazowy - wynik już zaraportowany w setup()
}
