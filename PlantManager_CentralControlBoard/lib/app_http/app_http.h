#ifndef APP_HTTP_H
#define APP_HTTP_H

#include "hal_radio.h"

void app_http_init();
bool app_http_publish(const PlantData &data);

#endif // APP_HTTP_H
