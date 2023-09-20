set(PLATFORM_C_FLAGS "-m64 -nostdlib -ffreestanding -g -mno-red-zone -lgcc")

# Set the platform-specific C++ flags for pc
set(PLATFORM_CXX_FLAGS "")

# Set the platform-specific ASM flags for pc
set(PLATFORM_ASM_FLAGS "")

# Set the platform-specific Linker flags for pc
# link lib gcc
set(PLATFORM_LINKER_FLAGS "-nostdlib -mno-red-zone -lgcc")
