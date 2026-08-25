#pragma once
#include <Arduino.h>

// Format pakietu test4_nrf_link - musi być identyczny (te same typy, ta sama kolejność
// pól, __attribute__((packed))) po obu stronach łącza radiowego, inaczej dane się rozjadą.
struct __attribute__((packed)) LinkTestPacket {
  uint32_t boardId;
  uint32_t counter;
};
