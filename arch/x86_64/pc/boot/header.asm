section .multiboot_header
header_start:
  dd 0xe85250d6 ; magic multboot2 header
  dd 0 ; architecture i386
  dd header_end - header_start ; header length
  dd 0 - (0xe85250d6 + 0 + (header_end - header_start)) ; checksum

  dw 0
  dw 0
  dw 8
header_end:
