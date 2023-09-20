#pragma once
#include <stddef.h>

namespace Terminal {
  void Init();
  void PutChar(char character);
  void WriteExact(const char *data, size_t size);
  void Write(const char *data);
}
