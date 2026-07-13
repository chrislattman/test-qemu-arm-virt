.section ".text.boot"
.global _start

_start:
    // Force CPU cores other than Core 0 into an infinite sleep loop
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, master_cpu
hang:
    wfe
    b       hang

master_cpu:
    // Set up a temporary safe stack pointer region inside RAM
    ldr     x0, =_stack_top
    mov     sp, x0

    // Jump straight to main architecture loop
    bl      main_app

    // Defensive fallback hang loop
    b       hang
