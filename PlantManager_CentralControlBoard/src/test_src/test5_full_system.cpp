#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "bsp_pins.h"
#include "secrets.h"
#include "radio_config.h"

// Zamrozona migawka dawnego main.cpp: radio -> WiFi -> MQTT, ze szczegolowymi logami serial
struct __attribute__((packed)) PlantData {
  uint32_t id;
  int16_t moisture;
};

RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
const byte address[6] = NRF_ADDRESS;
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Laczenie z WiFi: ");
  Serial.println(WIFI_SSID);

  // Skan przed polaczeniem - pozwala odroznic "sieci nie widac" od "zle haslo"
  int8_t networksFound = WiFi.scanNetworks();
  bool ssidVisible = false;
  for (int8_t i = 0; i < networksFound; i++) {
    if (WiFi.SSID(i) == WIFI_SSID) {
      ssidVisible = true;
      break;
    }
  }
  Serial.println(ssidVisible ? "Siec widoczna w skanie" : "UWAGA: sieci nie widac w skanie!");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint16_t attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(500);
    Serial.print(".");
    attempts++;

    if (attempts % 20 == 0) { // co ok. 10s diagnoza przyczyny niepowodzenia
      switch (WiFi.status()) {
        case WL_NO_SSID_AVAIL:
          Serial.println("\nBrak sieci o tej nazwie w zasiegu");
          break;
        case WL_CONNECT_FAILED:
          Serial.println("\nPolaczenie odrzucone - sprawdz haslo");
          break;
        default:
          Serial.println("\nDalej probuje polaczyc...");
      }
    }
  }

  // HiveMQ wymaga certyfikatow - setInsecure() pomija ich weryfikacje
  espClient.setInsecure();

  Serial.println("");
  Serial.println("WiFi polaczone");
  digitalWrite(LED_PIN, LOW);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Laczenie z MQTT...");

    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("polaczono!");
      digitalWrite(LED_PIN, LOW);
    } else {
      Serial.print("blad, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  if (!radio.begin() || !radio.isChipConnected()) {
    Serial.println(F("BLAD: radio niedostepne"));
    while (1) delay(100);
  }

  radio.setPALevel(NRF_PA_LEVEL);
  radio.setDataRate(NRF_DATA_RATE);
  radio.setChannel(NRF_CHANNEL);
  radio.openReadingPipe(1, address);
  radio.startListening();

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);

  digitalWrite(LED_PIN, LOW);
  Serial.println("System Monitoringu (ESP8266) Gotowy! [test5_full_system]");
}

void loop() {
  PlantData receivedData;

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));

    Serial.print("ID: ");
    Serial.print(receivedData.id);
    Serial.print(" | Wilg: ");
    Serial.println(receivedData.moisture);

    String jsonPayload = "{\"id\":" + String(receivedData.id) +
                         ",\"val\":" + String(receivedData.moisture) + "}";

    client.publish(MQTT_TOPIC_BASE, jsonPayload.c_str());
    Serial.print("Wyslano MQTT: ");
    Serial.println(jsonPayload.c_str());
  }
}
