#ifndef SENSOR_DEFINITIONS_H
#define SENSOR_DEFINITIONS_H

#include <Arduino.h>

struct __attribute__((packed)) plantData {
  uint32_t id;
  int16_t moisture;
};

void app_mqtt_publish_json(const struct plantData& data);
void setupHardware();
void setupRadio();

#endif