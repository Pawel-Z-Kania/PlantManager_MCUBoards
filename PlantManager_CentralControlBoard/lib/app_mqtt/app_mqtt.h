#pragma once
#include <Arduino.h>

void app_mqtt_init();
void app_mqtt_loop();
void app_mqtt_publish_json(uint32_t boardId, const String& jsonData);