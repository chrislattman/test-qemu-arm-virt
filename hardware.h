#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

// --- UART PL011 ---
#define UART0_BASE               0x09000000
#define UART0_DR                 ((volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_FR                 ((volatile uint32_t *)(UART0_BASE + 0x18))
#define UART_FR_TXFF             (1 << 5)

#endif // HARDWARE_H
