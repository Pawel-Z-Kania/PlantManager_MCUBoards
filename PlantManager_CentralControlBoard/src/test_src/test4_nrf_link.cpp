#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "bsp_pins.h"
#include "radio_config.h"
#include "link_test_protocol.h"

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
const byte address[6] = NRF_ADDRESS;

// Sledzimy ostatni licznik osobno dla kazdej plytki (boardId), zeby przy kilku
// jednoczesnych nadajnikach nie mylic ich strumieni ze soba przy wykrywaniu luk
const int MAX_TRACKED_BOARDS = 4;
uint32_t knownBoardIds[MAX_TRACKED_BOARDS];
uint32_t lastCounters[MAX_TRACKED_BOARDS];
int knownBoardCount = 0;

unsigned long lastPacketTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("\n--- test4_nrf_link: ODBIORNIK (NodeMCU) ---"));

  // Sprzęt jest sprawdzany osobno przez test3_nrf_chip - tu zakładamy, że przeszedł
  if (!radio.begin() || !radio.isChipConnected()) {
    Serial.println(F("BŁĄD: radio niedostępne - uruchom najpierw test3_nrf_chip"));
    while (1) delay(100);
  }

  radio.openReadingPipe(1, address);
  radio.setPALevel(NRF_PA_LEVEL);
  radio.setDataRate(NRF_DATA_RATE);
  radio.setChannel(NRF_CHANNEL);
  radio.setAutoAck(true);
  radio.startListening();

  Serial.println(F("Radio gotowe. Oczekiwanie na pakiety...\n"));
}

void loop() {
  if (radio.available()) {
    LinkTestPacket packet;
    radio.read(&packet, sizeof(packet));

    unsigned long now = millis();
    Serial.printf("Odebrano pakiet #%lu od plytki %lu", packet.counter, packet.boardId);

    int slot = -1;
    for (int i = 0; i < knownBoardCount; i++) {
      if (knownBoardIds[i] == packet.boardId) {
        slot = i;
        break;
      }
    }
    if (slot == -1 && knownBoardCount < MAX_TRACKED_BOARDS) {
      slot = knownBoardCount++;
      knownBoardIds[slot] = packet.boardId;
      lastCounters[slot] = 0;
    }

    if (slot != -1) {
      if (lastCounters[slot] > 0) {
        long gap = (long)packet.counter - (long)lastCounters[slot] - 1;
        if (gap > 0) {
          Serial.printf(" [UWAGA: Utracono %ld pakietow]", gap);
        }
      }
      lastCounters[slot] = packet.counter;
    }

    Serial.printf(" | Odstęp: %lu ms", now - lastPacketTime);
    Serial.println();

    lastPacketTime = now;
  }

  if (lastPacketTime > 0 && (millis() - lastPacketTime > 5000)) {
    Serial.println(F("⚠ Brak pakietów od >5 sekund. Sprawdź nadajnik."));
    lastPacketTime = millis();
  }
}
