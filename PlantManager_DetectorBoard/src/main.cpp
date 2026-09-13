#include <Arduino.h>
#include "sensor_definitions.h"
#include "config.h"
#include "bsp_pins.h"
#include "board_id.h"
#include "low_power.h"
#include "battery.h"

// SETUP I KONFIGURACJA ==================================================================================
plantData dataObject;

void setup()
{
  setupHardware();
  logResetReason();
  setupRadio();

  dataObject.id = getBoardId();

#if DEBUG_LOG
  Serial.print(F("[Boot] ID plytki: "));
  Serial.println(dataObject.id);
  // Ujawnia od razu, czy wczytany/domyslny interwal jest sensowny (diagnoza "nadawania w petli")
  Serial.print(F("[Boot] Interwal wysylania: "));
  Serial.print(getSensorTxIntervalMs() / 1000UL);
  Serial.println(F("s"));
#endif
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
  dataObject.batteryMv = readVccMv();

  if (dataObject.batteryMv < BATTERY_MIN_MV)
  {
#if DEBUG_LOG
    Serial.print(F("[Battery] Zbyt niskie napiecie ("));
    Serial.print(dataObject.batteryMv);
    Serial.println(F("mV), pomijam nadawanie"));
#endif
  }
  else
  {
    transmitSensorData(dataObject);
  }

  lowPowerSleepMs(getSensorTxIntervalMs());
}
