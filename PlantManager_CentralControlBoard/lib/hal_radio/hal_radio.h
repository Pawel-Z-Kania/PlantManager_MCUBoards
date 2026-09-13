#pragma once
#include <RF24.h>

struct __attribute__((packed)) PlantData {
  uint32_t id;
  int16_t moisture;
  uint16_t batteryMv;
};
static_assert(sizeof(PlantData) == 8, "PlantData layout must match Detector's plantData");

void hal_radio_init();
bool hal_radio_read(PlantData &data);
void hal_radio_set_config(uint32_t configVersion, uint16_t intervalSeconds);