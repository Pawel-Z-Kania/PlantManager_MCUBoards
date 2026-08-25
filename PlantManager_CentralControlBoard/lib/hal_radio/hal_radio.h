#pragma once
#include <RF24.h>

struct __attribute__((packed)) PlantData {
  uint32_t id;
  int16_t moisture;
};

void hal_radio_init();
bool hal_radio_read(PlantData &data);