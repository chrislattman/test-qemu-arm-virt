# Bare metal C example for QEMU ARM "virt" machine with Cortex-A53 processor

Run `sudo apt install gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu qemu-system-arm`

Use the provided Makefile, and quit QEMU by pressing Ctrl + A, release and then press X

Notes:

- If a watchdog timer is desired, the Intel 6300ESB `-device i6300esb` PCIe device is available to add to the `qemu-system-aarch64` command
- If you want to read UART messages from a /dev file, add `-serial pty` to the `qemu-system-aarch64` command, then run `screen /dev/pts/?` where ? is the value given by QEMU
- The difference between the `-bios` and `-kernel` QEMU flags is that `-bios` treats the binary as a physical ROM chip and boots at address 0x00000000, whereas `-kernel` makes QEMU act like a bootloader by performing some basic register initialization and loads the binary into address 0x40000000 as specified by the linker script linker.ld
