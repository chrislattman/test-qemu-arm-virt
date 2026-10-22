cmake_minimum_required(VERSION 3.27)

# Cross compiler for bare metal AArch64 (must be set before project())
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_ASM_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(baremetal C ASM)

set(CMAKE_C_FLAGS "-ffreestanding -mgeneral-regs-only -Wall -Wextra -pedantic")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE MinSizeRel CACHE STRING "Build type" FORCE)
endif()
set(CMAKE_C_FLAGS_DEBUG "-Og -ggdb3")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG")

# Link using standalone bypass flags
add_executable(baremetal.elf startup.S main.c)
target_link_options(baremetal.elf PRIVATE
    -nostdlib -static -no-pie -Wl,--build-id=none -T ${CMAKE_SOURCE_DIR}/linker.ld)

# Extract raw flat execution binary
add_custom_command(TARGET baremetal.elf POST_BUILD
    COMMAND aarch64-linux-gnu-objcopy -O binary baremetal.elf baremetal.bin)

add_custom_target(qemu
    COMMAND qemu-system-aarch64
        -M virt
        -cpu cortex-a53
        -nographic
        -serial telnet:localhost:5678,server=on,wait=off
        -kernel baremetal.bin
    DEPENDS baremetal.elf
    USES_TERMINAL
    VERBATIM
)

add_custom_target(qemu_debug
    COMMAND qemu-system-aarch64
        -M virt
        -cpu cortex-a53
        -nographic
        -serial telnet:localhost:5678,server=on,wait=off
        -kernel baremetal.bin
        -S
        -s
    DEPENDS baremetal.elf
    USES_TERMINAL
    VERBATIM
)
