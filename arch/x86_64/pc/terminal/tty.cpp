#include <veil/tty.hpp>
#include "vga.hpp"
#include <ssg.hpp>
#include <debug.hpp>

namespace Terminal {
static constexpr size_t VGA_WIDTH = 80;
static constexpr size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

template <typename T, size_t MaxSize>
class SimpleStack {
private:
    T data[MaxSize];
    size_t currentIndex;

public:
    SimpleStack() : currentIndex(0) {}

    // Push an item onto the stack
    void push(const T& value) {
        if (currentIndex < MaxSize) {
            data[currentIndex++] = value;
        } else {
            // Shift all items to the left, dropping the oldest
            for (size_t i = 0; i < MaxSize - 1; i++) {
                data[i] = data[i + 1];
            }
            data[MaxSize - 1] = value;
        }
    }

    // Pop an item from the stack
    T pop() {
        if (currentIndex > 0) {
            return data[--currentIndex];
        }
        // Default-construct if there's nothing to pop
        return T{};
    }

    // Peek at the top of the stack
    T top() const {
        if (currentIndex > 0) {
            return data[currentIndex - 1];
        }
        // Default-construct if there's nothing to peek
        return T{};
    }

    // Check if the stack is empty
    bool empty() const {
        return currentIndex == 0;
    }

    // Clear the stack
    void clear() {
        currentIndex = 0;
    }

    // Get the size (number of items) of the stack
    size_t size() const {
        return currentIndex;
    }
};

static size_t Row;
static size_t Column;
static uint8_t Color;
static VGA::Char* Buffer;
static SimpleStack<uint8_t, 16> colorStack;

bool HasBeenInitialized() {
  return Buffer != nullptr;
}

void ClearRow(size_t row) {
  auto empty = VGA::Char {
    character: ' ',
    color: Color,
  };

  for (size_t col = 0; col < VGA_WIDTH; col++) {
    Buffer[col + VGA_WIDTH * row] = empty;
  }
}

void ClearScreen() {
  for (size_t i = 0; i < VGA_HEIGHT; i++) {
    ClearRow(i);
  }
}

void Init() {
  Row = 0;
  Column = 0;
  Color = VGA::EntryColor(VGA::Color::LightGrey, VGA::Color::Black);
  Buffer = (VGA::Char*) VGA_MEMORY;
  ClearScreen();
  Log(LogLevel::Debug, "Terminal initialized");
}

void SetColor(uint8_t color) {
  Color = color;
}

void PrintNewline() {
  Column = 0;

  if (Row < VGA_HEIGHT - 1) {
    Row++;
    return;
  }

  for (size_t row = 1; row < VGA_HEIGHT; row++) {
    for (size_t col = 0; col < VGA_WIDTH; col++) {
      auto character = Buffer[Column + VGA_WIDTH * Row];
      Buffer[col + VGA_WIDTH * (Row - 1)] = character;
    }
  }

  ClearRow(VGA_HEIGHT - 1);
}

void PutEntryAt(unsigned char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  Buffer[index] = VGA::Entry(c, color);
}

void PutChar(char character) {
  unsigned char uc = character;

  if (uc == '\n') {
    PrintNewline();
    return;
  }

  if (Column >= VGA_WIDTH) {
    PrintNewline();
  }

  PutEntryAt(uc, Color, Column, Row);
  
  Column++;
}

uint8_t ParseNumber(const char* &data) {
  uint8_t value = 0;
  while (*data >= '0' && *data <= '9') {
    value = value * 10 + (*data - '0');
    data++;
  }
  data--;
  return value;
}

void ParseColorCode(const char* &data) {
  if (*data != '\033') {
    return;
  }
  data++; // skip '\033'

  if (*data == '[') {
    data++; // skip '['

    uint8_t fg = ParseNumber(data);
    uint8_t bg = (uint8_t) VGA::Color::Black; // default

    if (*(data + 1) == ';') {
      data++; // skip ';'
      bg = ParseNumber(data);
    } else {
      data++; // skip fg
    }

    uint8_t newColor = VGA::EntryColor(static_cast<VGA::Color>(fg), static_cast<VGA::Color>(bg));
    colorStack.push(Color);
    SetColor(newColor);
  } else if (*data == ']') {
    if (!colorStack.empty()) {
      uint8_t lastColor = colorStack.top();
      SetColor(lastColor);
      colorStack.pop();
    }
    data++; // skip ']'
  }
}

void WriteExact(const char* data, size_t size) {
  while (*data) {
    // if (*data == '\033') {
    //   ParseColorCode(data);
    // } else {
    //   PutChar(*data);
    //   data++;
    // }
    PutChar(*data);
    data++;
  }
}

size_t strlen(const char* str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

void Write(const char* data) {
  WriteExact(data, strlen(data));
}
}
