#pragma once

#include <stdint.h>

namespace VGA {
enum class Color {
  Black = 0,
  Blue = 1,
  Green = 2,
  Cyan = 3,
  Red = 4,
  Magenta = 5,
  Brown = 6,
  LightGrey = 7,
  DarkGrey = 8,
  LightBlue = 9,
  LightGreen = 10,
  LightCyan = 11,
  LightRed = 12,
  Pink = 13,
  Yellow = 14,
  White = 15,
};

struct Char {
  uint8_t character;
  uint8_t color;
} __attribute__((packed));

static inline uint8_t EntryColor(Color foreground, Color background) {
  return ((uint8_t)foreground) | ((uint8_t)background) << 4;
}

static inline Char Entry(unsigned char uc, uint8_t color) {
  return Char {
    .character = uc,
    .color = color,
  };
}
}
