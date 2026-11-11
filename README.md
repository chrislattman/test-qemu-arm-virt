# Bare metal C example for QEMU ARM "virt" machine with Cortex-A53 processor

## Prerequisites

Run `sudo apt install qemu-system-arm cmake gdb-multiarch`

### Toolchain: Arm GNU Toolchain, `aarch64-none-elf`

The build uses Arm's bare-metal AArch64 toolchain. Ubuntu doesn't package it, so download it from Arm:

1. Go to https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
2. Under **x86_64 Linux hosted cross toolchains**, download **AArch64 bare-metal target (aarch64-none-elf)** (`arm-gnu-toolchain-<version>-x86_64-aarch64-none-elf.tar.xz`). Optionally also download the `.sha256asc` file and compare it with `sha256sum` on the tarball.
3. Extract it and add its `bin` directory to your `PATH`, for example:

   ```
   mkdir -p ~/.local/arm-gnu-toolchain
   tar -xJf arm-gnu-toolchain-<version>-x86_64-aarch64-none-elf.tar.xz -C ~/.local/arm-gnu-toolchain
   echo 'export PATH="$HOME/.local/arm-gnu-toolchain/arm-gnu-toolchain-<version>-x86_64-aarch64-none-elf/bin:$PATH"' >> ~/.bashrc
   source ~/.bashrc
   aarch64-none-elf-gcc --version
   ```

## Build and run

```
cmake -S . -B build
cmake --build build --target qemu       # connect with `telnet localhost 5678`
cmake --build build --target qemu_debug # debug
```

## Notes

- If a watchdog timer is desired, the Intel 6300ESB `-device i6300esb` PCIe device is available to add to the `qemu-system-aarch64` command
- If you want to read UART messages from a /dev file, add `-serial pty` to the `qemu-system-aarch64` command, then run `screen /dev/pts/?` where ? is the value given by QEMU
- The difference between the `-bios` and `-kernel` QEMU flags is that `-bios` treats the binary as a physical ROM chip and boots at address 0x00000000, whereas `-kernel` makes QEMU act like a bootloader by performing some basic register initialization and loads a raw binary into address 0x40080000 (RAM base + 0x80000, with the device tree at the RAM base), which matches the linker script linker.ld

## Compiler flags and the Ubuntu cross compiler

This project used to build with Ubuntu's `aarch64-linux-gnu-gcc` (`gcc-aarch64-linux-gnu` package). That compiler targets Linux, so the build needed extra link flags to undo its Linux defaults. With `aarch64-none-elf` they are redundant (the output binary is byte-for-byte identical without them), so they were removed:

- `-static`: bare-metal ELF output is always static.
- `-no-pie`: Ubuntu's GCC builds position-independent executables by default; `aarch64-none-elf` doesn't.
- `-Wl,--build-id=none`: Ubuntu's GCC tells the linker to add a `.note.gnu.build-id` section; `aarch64-none-elf` doesn't.

If you ever go back to `aarch64-linux-gnu-gcc`, add those three flags back to `target_link_options`. Also note that Ubuntu's GCC turns on `-fstack-protector-strong` by default; `-ffreestanding` happens to turn it off, but if it ever comes back, the link fails with undefined `__stack_chk_fail`/`__stack_chk_guard`, so add `-fno-stack-protector`.
