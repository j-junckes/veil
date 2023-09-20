global long_mode_start
global stack_bottom
global stack_top
extern KernelMain
extern _init

section .text
bits 64
long_mode_start:
  ; load null into all data segment registers
  mov ax, 0
  mov ss, ax
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  call _init
  mov rsp, stack_top

  ; clean stack area
  mov rdi, stack_bottom
  mov rcx, 4096 * 4
  mov al, 0
  rep stosb

  call KernelMain
  
  cli
  hlt

section .bss
align 4096
stack_bottom:
resb 4096 * 4
stack_top:
