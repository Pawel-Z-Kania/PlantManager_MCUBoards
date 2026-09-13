#pragma once

#include <Arduino.h>

constexpr uint32_t CONFIG_PACKET_MAGIC = 0x504D4346UL;
constexpr uint16_t CONFIG_PACKET_PROTOCOL_VERSION = 1;
constexpr uint32_t CONFIG_BROADCAST_BOARD_ID = 0;

struct __attribute__((packed)) RadioConfigPacket {
    uint32_t magic;
    uint16_t protocolVersion;
    uint32_t configVersion;
    uint32_t targetBoardId;
    uint16_t sensorSampleIntervalSec;
};

static_assert(sizeof(RadioConfigPacket) <= 32, "nRF24 ACK payload must fit in 32 bytes");