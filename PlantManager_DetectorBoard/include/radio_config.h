#pragma once
#include <RF24.h>

// Parametry radiowe - muszą być identyczne w obu projektach (Central i Detector), inaczej TX/RX się rozjadą
#define NRF_ADDRESS "00001"
#define NRF_CHANNEL 76
#define NRF_PA_LEVEL RF24_PA_LOW
#define NRF_DATA_RATE RF24_1MBPS
