#include <veil/serial.hpp>
#include <veil/io.hpp>
#include <string.hpp>
#include <debug.hpp>

namespace Serial {
  constexpr uint16_t COM_1_PORT = 0x3f8;

  static bool initialized = false;

  int BytesAvailable() {
    return IO::InByte(COM_1_PORT + 5) & 1;
  }

  int IsTransmitEmpty() {
    return IO::InByte(COM_1_PORT + 5) & 0x20;
  }

  int Init() {
    IO::OutByte(COM_1_PORT + 1, 0x00); // Disable all interrupts
    IO::OutByte(COM_1_PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    IO::OutByte(COM_1_PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    IO::OutByte(COM_1_PORT + 1, 0x00); //                  (hi byte)
    IO::OutByte(COM_1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    IO::OutByte(COM_1_PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    IO::OutByte(COM_1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
    IO::OutByte(COM_1_PORT + 4, 0x1E); // Set in loopback mode, test the serial chip
    IO::OutByte(COM_1_PORT + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if(IO::InByte(COM_1_PORT + 0) != 0xAE) {
      Log(LogLevel::Error, "Serial malfunctioning");
      return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    IO::OutByte(COM_1_PORT + 4, 0x0F);

    initialized = true;
    Log(LogLevel::Debug, "Serial initialized");
    return 0;
  }

  void Write(const char *str) {
    auto len = StrLength(str);

    for (int i = 0; i < len; i++) {
      WriteByte(str[i]);
      if (str[i] == '\n') {
        WriteByte('\r');
      }
    }
  }

  void Write(size_t number) {
    char str[256];
    IntToString(number, str, 10);
    Write(str);
  }

  void WriteHex(size_t number) {
    char str[256];
    IntToString(number, str, 16);
    Write(str);
  }

  bool HasBeenInitialized() {
    return initialized;
  }

  void WriteByte(char c) {
    while (IsTransmitEmpty() == 0);

    IO::OutByte(COM_1_PORT, c);
  }

  char *Read() {
    // char *str = new char[256];
    // int i = 0;

    // while (true) {
    //   char c = ReadByte();
    //   if (c == '\n') {
    //     str[i] = '\0';
    //     break;
    //   }
    //   str[i] = c;
    //   i++;
    // }

    // return str;
    return nullptr;
  }

  char ReadByte() {
    while (BytesAvailable() == 0);

    return IO::InByte(COM_1_PORT);
  }
}
