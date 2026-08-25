#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include "bsp_pins.h"
#include "board_id.h"
#include "radio_config.h"
#include "link_test_protocol.h"

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
const byte address[6] = NRF_ADDRESS;

LinkTestPacket packet;

void setup() {
  Serial.begin(9600);
  delay(1000);
  printf_begin();
  Serial.println(F("\n--- test4_nrf_link: NADAJNIK (Arduino) ---"));

  packet.boardId = getBoardId();
  packet.counter = 0;
  Serial.print(F("ID płytki: "));
  Serial.println(packet.boardId);

  // Sprzęt jest sprawdzany osobno przez test3_nrf_chip - tu zakładamy, że przeszedł
  if (!radio.begin() || !radio.isChipConnected()) {
    Serial.println(F("BŁĄD: radio niedostępne - uruchom najpierw test3_nrf_chip"));
    while (1) delay(100);
  }

  radio.openWritingPipe(address);
  radio.setChannel(NRF_CHANNEL);
  radio.setPALevel(NRF_PA_LEVEL);
  radio.setDataRate(NRF_DATA_RATE);
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  radio.stopListening();

  Serial.println(F("Radio gotowe. Rozpoczynam nadawanie...\n"));
}

void loop() {
  packet.counter++;

  if (radio.failureDetected) {
    Serial.println(F("WYKRYTO AWARIĘ RADIA! Reset..."));
    radio.begin();
    radio.openWritingPipe(address);
    radio.stopListening();
    radio.failureDetected = 0;
    delay(100);
    return;
  }

  unsigned long startTime = millis();
  bool success = radio.write(&packet, sizeof(packet));
  unsigned long duration = millis() - startTime;
  uint8_t retries = radio.getARC(); // liczba retransmisji zuzytych na ostatni pakiet (0-15)

  Serial.print(F("Wysyłam pakiet nr: "));
  Serial.print(packet.counter);
  Serial.print(F(" (czas: "));
  Serial.print(duration);
  Serial.print(F("ms, ARC: "));
  Serial.print(retries);
  Serial.print(F("/15) -> "));
  Serial.println(success ? F("SUKCES") : F("BŁĄD"));

  delay(1000);
}
