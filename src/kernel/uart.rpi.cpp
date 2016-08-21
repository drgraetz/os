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

uint8_t readByte(unsigned int port) {
    return -1;
}

void writeByte(unsigned int port, uint8_t value) {
}

void writeWord(unsigned int port, uint16_t value) {
}

}

#endif
