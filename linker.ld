ENTRY(_start)

SECTIONS
{
    . = 0x40000000; /* RAM Base Address */

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
