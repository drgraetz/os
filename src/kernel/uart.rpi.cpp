#ifdef VERBOSE
#include <stdint.h>

#include "kernel.hpp"

/**
 * The GPIO base port for the raspberry pi. The addresses of the peripherals
 * start at 0x20000000 for the BCM2708 (would be 0x3F000000 for the raspberry
 * pi 2). The GPI header is located at the position 0x200000 within the
 * periphery.
 *
 * A detailed description of the registers can be found here:
 * https://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
 */
// 0x3F200000 for rasppi2
#define BASEPORT        0x20200000

enum {
    GPPUD=0x94,         ///< actuation of pull up / pull down to ALL pins
    GPPUDCLK0=0x98,     ///< actuation of pull up / pull down or specific pin
    UART_DATA=0x1000,   ///< UART data register
    UART_RSRECR=0x1004, ///< UART data register
    UART_FLAGS=0x1018,  ///< UART flag register
    UART_IBRD=0x1024,   ///< UART integer baud rate divisor
    UART_FBRD=0x1028,   ///< UART fractional baud rate divisor
    UART_LCR=0x102C,    ///< UART line control register
    UART_CR=0x1030,     ///< UART control register
    UART_IFLS=0x1034,   ///< UART interrupt FIFO level select register
    UART_IMSC=0x1038,   ///< UART interrupt mask set clear register
    UART_RIS=0x103C,    ///< UART raw interrupt status register
    UART_MIS=0x1040,    ///< UART masked interrupt status register
    UART_ICR=0x1044,    ///< UART interrupt clear register
    UART_DMACR=0x1048,  ///< UART DMA control register
    UART_ITCR=0x1080,   ///< UART test control register
    UART_ITIP=0x1084,   ///< UART integration test input register
    UART_ITOP=0x1088,   ///< UART integration test output regsiter
    UART_TDR=0x108C,    ///< UART test data register
};
/*
#define GPFSEL1         (PBASE+0x00200004)
#define GPSET0          (PBASE+0x0020001C)
#define GPCLR0          (PBASE+0x00200028)
#define GPPUD           (PBASE+0x00200094)
#define GPPUDCLK0       (PBASE+0x00200098)

#define AUX_ENABLES     (PBASE+0x00215004)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_IER_REG  (PBASE+0x00215044)
#define AUX_MU_IIR_REG  (PBASE+0x00215048)
#define AUX_MU_LCR_REG  (PBASE+0x0021504C)
#define AUX_MU_MCR_REG  (PBASE+0x00215050)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)
#define AUX_MU_MSR_REG  (PBASE+0x00215058)
#define AUX_MU_SCRATCH  (PBASE+0x0021505C)
#define AUX_MU_CNTL_REG (PBASE+0x00215060)
#define AUX_MU_STAT_REG (PBASE+0x00215064)
#define AUX_MU_BAUD_REG (PBASE+0x00215068)

typedef unsigned int uint32_t;

extern "C" void __aeabi_unwind_cpp_pr0() {
}

static inline uint32_t get32(uint32_t port) {
    uint32_t result;
    asm(
        "ldr    %1, [%0]" :
        "=r"(port) :
        "r"(result)
    );
    return result;
}

static inline void put32(uint32_t port, uint32_t value) {
    asm(
        "str    %1, [%0]" :
        "=r"(port), "=r"(value)
    );
}

int dummy(int value) {
    return value * 2;
}

void init_debug_interface() {
    put32(AUX_ENABLES, 1);
    put32(AUX_MU_IER_REG, 0);
    put32(AUX_MU_CNTL_REG, 0);
    put32(AUX_MU_LCR_REG, 3);
    put32(AUX_MU_MCR_REG, 0);
    put32(AUX_MU_IER_REG, 0);
    put32(AUX_MU_IIR_REG, 0xC6);
    put32(AUX_MU_BAUD_REG, 270);

    unsigned int ra = get32(GPFSEL1);
    ra &= ~(7 << 12);       //gpio14
    ra |= 2 << 12;          //alt5
    ra &= ~(7 << 15);       //gpio15
    ra |= 2 << 15;          //alt5
    put32(GPFSEL1, ra);


    put32(GPPUD,0);
    for (ra=0; ra < 150; ra++) dummy(ra);
    put32(GPPUDCLK0, (1 << 14) | (1 << 15));
    for (ra=0; ra < 150; ra++) dummy(ra);
    put32(GPPUDCLK0, 0);

    put32(AUX_MU_CNTL_REG, 3);
}

int putchar(int value) {
    if ((value < 32 || value > 127) && value != '\r' && value != '\n' &&
        value != '\t') {
        value = 127;
    }
    while (true) {
        if (get32(AUX_MU_LSR_REG) & 0x20) break;
    }
    put32(AUX_MU_IO_REG, value);
    return value;
}
*/
namespace uart {

static inline void delay(int32_t count) {
    asm volatile(
        "__delay_%=: subs %[count], %[count], #1\r\n"
        "bne __delay_%=\r\n" :
        "=r"(count) :
        [count]"0"(count) :
        "cc"
    );
}

static inline void write(unsigned int reg, uint32_t value) {
    *(uint32_t*)(BASEPORT + reg) = value;
}

static inline uint32_t read(unsigned int reg) {
    return *(uint32_t*)(BASEPORT + reg);
}

bool isReadyToSend() {
    return !(read(UART_FLAGS) & 0x20);
}

void send(char c) {
    write(UART_DATA, c & 0xFF);
}

void init() {
    // disable UART
    write(UART_CR, 0);
    // disable pull up / pull down for all GPIO pins
    write(GPPUD, 0);
    delay(150);
    // disable pull up / pull down for pins 14 & 15
    write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);
    write(GPPUDCLK0, 0);
    // clear pending interrupts
    write(UART_ICR, 0x7FF);
    // divider = 187500 / baud rate
    // fractional part = divider * 64 + 0.5
    // for 9.600 baud: divider = 19.53125
    // fractional part = 34.5
    write(UART_IBRD, 19);
    write(UART_FBRD, 35);
    // enable FIFO, 8 data bits, 1 stop bit, no parity
    write(UART_LCR, 0x70);
    // mask all interrupts
    write(UART_IMSC, 0x7F2);
    // enable UART send & receive
    write(UART_CR, 0x301);
}

}

CDECL unsigned int __udivsi3(unsigned int a, unsigned int b) {
    size_t count = 1;
    unsigned int mask = 1;
    while (b < 0x80000000) {
        b = b << 1;
        mask = mask << 1;
        count++;
    }
    unsigned int result = 0;
    for (; count > 0; count--) {
        if (a >= b) {
            a -= b;
            result += mask;
        }
        b = b >> 1;
        mask = mask >> 1;
    }
    return result;
}

#endif
