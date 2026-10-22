default:
	# Assemble and Compile
	aarch64-linux-gnu-as startup.S -o startup.o
	aarch64-linux-gnu-gcc -c main.c -o main.o -ffreestanding -mgeneral-regs-only -Wall -Wextra -pedantic

	# Link using standalone bypass flags
	aarch64-linux-gnu-ld -T linker.ld startup.o main.o -o baremetal.elf

	# Extract raw flat execution binary
	aarch64-linux-gnu-objcopy -O binary baremetal.elf baremetal.bin

run: default
	qemu-system-aarch64 \
		-M virt \
		-cpu cortex-a53 \
		-nographic \
		-serial telnet:localhost:5678,server=on,wait=off \
		-kernel baremetal.bin

clean:
	rm -f *.bin *.elf *.o *.img
