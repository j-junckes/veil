#pragma once
#include <stdint.h>

namespace IO {
void OutByte(uint16_t port, uint8_t val);
uint8_t InByte(uint16_t port);
}
