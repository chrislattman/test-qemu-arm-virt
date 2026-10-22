#include "hardware.h"

// Shared variable across foreground code and async ISR layers
static volatile uint32_t shared_data = 0;

void uart_puts(const char *s) {
    while (*s) {
        // Wait until UART TX FIFO has space
        while (*UART0_FR & UART_FR_TXFF);
        *UART0_DR = *s++;
    }
}

void sleep_ms(uint32_t ms) {
    uint64_t frq, start, current;
    uint64_t ticks_needed;

    // Read System Counter Frequency
    asm volatile("mrs %0, cntfrq_el0" : "=r"(frq));
    ticks_needed = (frq / 1000) * ms;

    // Read Current System Counter Value
    asm volatile("mrs %0, cntpct_el0" : "=r"(start));
    do {
        asm volatile("mrs %0, cntpct_el0" : "=r"(current));
    } while ((current - start) < ticks_needed);
}

static inline void enable_interrupts(void)  { asm volatile("msr daifclr, #2" ::: "memory"); }
static inline void disable_interrupts(void) { asm volatile("msr daifset, #2" ::: "memory"); }

void trigger_sgi(uint32_t sgi_id) {
    // Bits [25:24]: Target List Filter (0b10 = forward strictly to the current requesting CPU core)
    // Bits [3:0]: SGI Interrupt ID Number
    *GICD_SGIR = (2 << 24) | (sgi_id & 0xF);
}

// The Unified C-Level Interrupt Vector Handler
void handle_irq(void) {
    // Read the Interrupt Acknowledge Register to get the pending ID
    uint32_t iar = *GICC_IAR;
    uint32_t interrupt_id = iar & 0x3FF; // Isolate lower 10 bits

    if (interrupt_id == HW_INT_SGI_ID) {
        uart_puts("[ISR] 'Hardware' Interrupt tripped! Writing data to shared space...\r\n");
        shared_data = 0xAABBCCDD; // Write to shared variable

        // Instantly trigger our secondary nested "Software" interrupt handling line
        trigger_sgi(SW_INT_SGI_ID);
    } 
    else if (interrupt_id == SW_INT_SGI_ID) {
        uart_puts("[ISR] 'Software' Interrupt tripped! Processing data inside a Critical Section...\r\n");

        // --- CRITICAL SECTION BEGIN ---
        disable_interrupts(); // Lock out incoming requests safely
        uint32_t local_copy = shared_data;

        // Print proof of successful read
        uart_puts("      Critical Read complete! Value is: 0x");

        // Simple hex print helper routine
        char hex[9];
        for (int i = 7; i >= 0; i--) {
            uint32_t nibble = (local_copy >> (i * 4)) & 0xF;
            hex[7 - i] = (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
        }
        hex[8] = '\0';
        uart_puts(hex);
        uart_puts("\r\n");

        enable_interrupts();
        // --- CRITICAL SECTION END ---
    }

    // Signal completion by writing back the processed ID to End of Interrupt register
    *GICC_EOIR = iar;
}

void gic_init(void) {
    *GICD_CTLR = 0; // Disable distributor for configuration
    *GICC_CTLR = 0; // Disable CPU interface for configuration

    // Leave SGIs in Group 0 (the reset default). GICD_CTLR/GICC_CTLR = 1 only
    // enable Group 0, and Group 1 interrupts can't be acknowledged via GICC_IAR
    // without GICC_CTLR.AckCtl, which would cause an endless IRQ storm.

    // Enable SGI 1 and SGI 2 (Bits 1 and 2 in the first ISENABLER register)
    *GICD_ISENABLER |= (1 << HW_INT_SGI_ID) | (1 << SW_INT_SGI_ID);

    // Set Priority Mask Register to allow all interrupts through (0xFF)
    *GICC_PMR = 0xFF;

    // Turn both the distributor and CPU interface back on
    *GICD_CTLR = 1;
    *GICC_CTLR = 1;
}

void main_app(void) {
#ifdef NDEBUG
    gic_init();
    enable_interrupts(); // Unmask IRQs inside the Cortex-A53 core
#endif
    while (1) {
        uart_puts("Hello\r\n");
#ifdef NDEBUG
        sleep_ms(1000);
#endif
        uart_puts("World!\r\n");
#ifdef NDEBUG
        trigger_sgi(HW_INT_SGI_ID); // Kick off the hardware -> software cascade
        sleep_ms(1000);
#endif
    }
}
