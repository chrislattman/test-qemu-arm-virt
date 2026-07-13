#include "hardware.h"

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

void main_app(void) {
    while (1) {
        uart_puts("Hello\r\n");
        sleep_ms(1000);
        uart_puts("World!\r\n");
        sleep_ms(1000);
    }
}
