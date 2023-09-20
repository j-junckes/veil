#include <io.hpp>
#include <veil/tty.hpp>
#include <veil/serial.hpp>
#include <stdint.h>
#include <debug.hpp>
#include <veil/io.hpp>

struct IdtEntry {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t	    ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
	uint32_t    isr_high;     // The higher 32 bits of the ISR's address
	uint32_t    reserved;     // Set to zero
} __attribute__((packed)) ;

__attribute__((aligned(0x10))) 
static IdtEntry idt[256]; // Create an array of IDT entries; aligned for performance

struct IdtR {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed));

static IdtR idtr;

struct interrupt_frame {
  uint64_t ip;
  uint64_t cs;
  uint64_t flags;
  uint64_t sp;
  uint64_t ss;
} __attribute__((packed));

extern "C" __attribute__((noreturn)) void interrupt_handler(interrupt_frame* frame) {
  Serial::Write("Interrupt!\n");
  Serial::Write("  IP: ");
  Serial::WriteHex(frame->ip);
  Serial::Write("\n");
  Serial::Write("  CS: ");
  Serial::WriteHex(frame->cs);
  Serial::Write("\n");
  Serial::Write("  Flags: ");
  Serial::WriteHex(frame->flags);
  Serial::Write("\n");
  Serial::Write("  SP: ");
  Serial::WriteHex(frame->sp);
  Serial::Write("\n");
  Serial::Write("  SS: ");
  Serial::WriteHex(frame->ss);
  Serial::Write("\n");
  //__asm__ volatile ("cli; hlt"); // Completely hangs the computer
}

// extern "C" __attribute__((interrupt)) void interrupt_handler(struct interrupt_frame* frame) {
//   Serial::Write("Interrupt!\n");
//   __asm__ volatile ("cli; hlt");
// }

extern uint16_t GDT_OFFSET_KERNEL_CODE;

void IdtSetDescriptor(uint8_t vector, void* isr, uint8_t flags) {
  IdtEntry* descriptor = &idt[vector];

  descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
  descriptor->kernel_cs      = GDT_OFFSET_KERNEL_CODE;
  descriptor->ist            = 0;
  descriptor->attributes     = flags;
  descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
  descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
  descriptor->reserved       = 0;
}

extern void* isr_stub_table[];

void IdtInit() {
  idtr.base = (uintptr_t)&idt[0];
  idtr.limit = (uint16_t)sizeof(IdtEntry) * 256 - 1;

  for (uint8_t vector = 0; vector < 32; vector++) {
    IdtSetDescriptor(vector, isr_stub_table[vector], 0x8E);
    //vectors[vector] = true;
  }

  __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
  __asm__ volatile ("sti"); // set the interrupt flag
}

constexpr uint16_t PS2_DATA_PORT = 0x60;
constexpr uint16_t PS2_STATUS_PORT = 0x64;
constexpr uint16_t PS2_COMMAND_PORT = 0x64;

extern "C" void KernelMain() {
  Serial::Init();
  Terminal::Init();

  IdtInit();

  // log all of isr_stub_table
  for (int i = 0; i < 32; i++) {
    Serial::Write("isr_stub_table[");
    Serial::Write(i);
    Serial::Write("] = ");
    Serial::WriteHex((uint64_t)isr_stub_table[i]);
    Serial::Write("\n");
  }

  // disable interrupts
  //asm volatile("cli");

  // enable ps2 keyboard interrupt
  IO::OutByte(PS2_COMMAND_PORT, 0xAE);
  IO::OutByte(PS2_COMMAND_PORT, 0x20);
  uint8_t status = IO::InByte(PS2_DATA_PORT) | 1;
  IO::OutByte(PS2_COMMAND_PORT, 0x60);
  IO::OutByte(PS2_DATA_PORT, status);

  // enable interrupts
  //asm volatile("sti");

  Print("Hello, kernel World!\n");

  while(true) {
    __asm__ volatile ("nop");
  }
}
