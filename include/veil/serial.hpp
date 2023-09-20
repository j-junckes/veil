#pragma once
#include <stddef.h>

namespace Serial {
  int Init();
  void Write(const char *str);
  void Write(size_t number);
  void WriteHex(size_t number);
  void WriteByte(char c);
  char *Read();
  char ReadByte();

  bool HasBeenInitialized();
}
