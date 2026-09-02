#ifndef SENSOR_DEFINITIONS_H
#define SENSOR_DEFINITIONS_H

#include <Arduino.h>

struct __attribute__((packed)) plantData {
  uint32_t id;
  int16_t moisture;
  uint16_t batteryMv;
};
static_assert(sizeof(plantData) == 8, "plantData layout must match Central's PlantData");

void app_mqtt_publish_json(const struct plantData& data);
void setupHardware();
void setupRadio();

#endif