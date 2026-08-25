#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "bsp_pins.h"
#include "board_id.h"
#include "config.h"
#include "radio_config.h"

// Zamrozona migawka dawnego main.cpp: czujnik -> radio, ze szczegolowymi logami serial
struct __attribute__((packed)) plantData {
  uint32_t id;
  int16_t moisture;
};

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
const byte address[6] = NRF_ADDRESS;
plantData dataObject;

void setup() {
  pinMode(PIN_MOISTURE_SENSOR, INPUT);
  pinMode(PIN_MOISTURE_POWER, OUTPUT);
  digitalWrite(PIN_MOISTURE_POWER, LOW);

  Serial.begin(SERIAL_BAUD);
  delay(1000);
  Serial.println(F("\n--- test5_full_system: Detector (czujnik + radio + log serial) ---"));

  dataObject.id = getBoardId();
  Serial.print(F("ID plytki: "));
  Serial.println(dataObject.id);

  if (!radio.begin()) {
    Serial.println(F("Nie wykryto radia nRF24! Sprawdz kable."));
    while (1);
  }

  radio.setPALevel(NRF_PA_LEVEL);
  radio.setDataRate(NRF_DATA_RATE);
  radio.setChannel(NRF_CHANNEL);
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println(F("=== Konfiguracja nRF24 ==="));
  Serial.print(F("Kanal: "));
  Serial.println(radio.getChannel());
  Serial.print(F("Data Rate: "));
  Serial.println(radio.getDataRate());
  Serial.print(F("PA Level: "));
  Serial.println(radio.getPALevel());
  Serial.println(F("=========================="));
}

void loop() {
  digitalWrite(PIN_MOISTURE_POWER, HIGH);
  delay(100);
  dataObject.moisture = analogRead(PIN_MOISTURE_SENSOR);
  digitalWrite(PIN_MOISTURE_POWER, LOW);

  bool sukces = radio.write(&dataObject, sizeof(dataObject));

  Serial.print(F("Wysylam ID: "));
  Serial.print(dataObject.id);
  Serial.print(F(" Wilg: "));
  Serial.print(dataObject.moisture);
  Serial.println(sukces ? F(" -> Wyslano OK") : F(" -> Blad wysylania (brak odbiornika?)"));

  delay(5000);
}
