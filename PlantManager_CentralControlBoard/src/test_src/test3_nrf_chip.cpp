#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "bsp_pins.h"
#include "radio_config.h"
#include "self_test.h"

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  selfTestSuiteBegin("test3_nrf_chip");

  bool beginOk = radio.begin();
  selfTestReport(beginOk, "radio.begin() powiodło się");

  bool chipOk = beginOk && radio.isChipConnected();
  selfTestReport(chipOk, "NRF24 odpowiada przez SPI (isChipConnected)");

  bool allPassed = beginOk && chipOk;
  if (allPassed) {
    radio.setChannel(NRF_CHANNEL);
    Serial.println(F("\n=== Konfiguracja radia ==="));
    radio.printDetails();
    Serial.println(F("=== Koniec konfiguracji ===\n"));
  } else {
    Serial.println(F("Sprawdź połączenia SPI oraz piny CE/CSN z bsp_pins.h"));
  }

  selfTestSuiteEnd(allPassed);
}

void loop() {
  // Test jednorazowy - wynik już zaraportowany w setup()
}
