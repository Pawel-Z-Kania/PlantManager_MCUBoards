#include <Arduino.h>
#include "sensor_definitions.h"
#include "config.h"
#include "bsp_pins.h"
#include "board_id.h"

// SETUP I KONFIGURACJA ==================================================================================
plantData dataObject;

void setup()
{
  setupHardware();
  setupRadio();

  dataObject.id = getBoardId();
}

// MAIN LOOP ==================================================================================
void loop()
{
  // Włącz zasilanie czujnika
  digitalWrite(PIN_MOISTURE_POWER, HIGH);
  delay(100); // Daj czas na stabilizację (100ms)
  
  // Wykonaj pomiar
  dataObject.moisture = analogRead(PIN_MOISTURE_SENSOR);
  
  // Wyłącz zasilanie czujnika
  digitalWrite(PIN_MOISTURE_POWER, LOW);

  // Wyślij dane
  app_mqtt_publish_json(dataObject);

  delay(SENSOR_TX_INTERVAL_MS);
}
