set(ISA_C_FLAGS "-m64 -nostdlib -ffreestanding -ggdb3 -O0 -mno-red-zone -lgcc -fstack-protector-all -fstack-protector-strong")

# Set the generic x86_64 C++ flags
set(ISA_CXX_FLAGS "-m64 -nostdlib -ffreestanding -ggdb3 -O0 -mno-red-zone -lgcc -fstack-protector-all -fstack-protector-strong")

# Set the generic x86_64 ASM flags
set(ISA_ASM_FLAGS "-f elf64")

# Set the generic x86_64 Linker flags, if any (this is just an example, you might not need linker flags here)
set(ISA_LINKER_FLAGS "-lgcc -no-pie -g -ffreestanding")
