#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

// --- UART PL011 ---
#define UART0_BASE               0x09000000
#define UART0_DR                 ((volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_FR                 ((volatile uint32_t *)(UART0_BASE + 0x18))
#define UART_FR_TXFF             (1 << 5)

// --- GICv2 (QEMU virt Memory Map) ---
#define GIC_DIST_BASE    0x08000000  // Distributor Base Address
#define GIC_CPU_BASE     0x08010000  // CPU Interface Base Address

// Distributor Registers
#define GICD_CTLR        ((volatile uint32_t *)(GIC_DIST_BASE + 0x000))
#define GICD_IGROUPR     ((volatile uint32_t *)(GIC_DIST_BASE + 0x080))
#define GICD_ISENABLER   ((volatile uint32_t *)(GIC_DIST_BASE + 0x100))
#define GICD_SGIR        ((volatile uint32_t *)(GIC_DIST_BASE + 0xF00))

// CPU Interface Registers
#define GICC_CTLR        ((volatile uint32_t *)(GIC_CPU_BASE + 0x000))
#define GICC_PMR         ((volatile uint32_t *)(GIC_CPU_BASE + 0x004))
#define GICC_IAR         ((volatile uint32_t *)(GIC_CPU_BASE + 0x00C))
#define GICC_EOIR        ((volatile uint32_t *)(GIC_CPU_BASE + 0x010))

// Assigning SGI ID IDs
#define HW_INT_SGI_ID    1
#define SW_INT_SGI_ID    2

#endif // HARDWARE_H
