ENTRY(_start)

SECTIONS
{
    . = 0x40080000; /* RAM base + 0x80000: where QEMU loads a raw -kernel image (DTB sits at RAM base) */

    .text : {
        *(.text.boot)
        *(.text)
    }

    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }

    . = ALIGN(16);
    . += 0x4000; /* Allocate 16KB stack space */
    _stack_top = .;
}
