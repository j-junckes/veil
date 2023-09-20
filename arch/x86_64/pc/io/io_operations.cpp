#include <veil/io.hpp>

namespace IO {
  void OutByte(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port) :"memory");
  }

  uint8_t InByte(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port)
                    : "memory");
    return ret;
  }
}
