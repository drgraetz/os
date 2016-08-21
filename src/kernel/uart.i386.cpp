#ifdef VERBOSE
#include "kernel.hpp"

namespace uart {

/**
 * The base port of the first serial interface.
 */
#define BASEPORT        0x3F8

static inline void writeByte(unsigned int port, uint8_t value) {
    asm(
        "outb   %%al, %%dx" : :
        "d"(BASEPORT + port), "a"(value)
    );
}

static inline void writeWord(unsigned int port, uint16_t value) {
    asm(
        "outw   %%ax, %%dx" : :
        "d"(BASEPORT + port), "a"(value)
    );
}

static inline uint8_t readByte(unsigned int port) {
    uint8_t result;
    asm(
        "inb    %%dx, %%al" :
        "=a"(result) :
        "d"(BASEPORT + port)
    );
    return result;
}

bool isReadyToSend() {
    return readByte(5) & 0x20;
}

void send(char c) {
    writeByte(0, c);
}

void init() {
    using namespace uart;
    // 8 bits, no parity, 1 stop bit, access divisor latch
    writeByte(3, 0x83);
    // speed 9600 baud
    uint16_t divisor = (uint16_t)(115200 / 9600);
    writeWord(0, divisor);
    // 8 bits, no parity, 1 stop bit
    writeByte(3, 0x03);
    // disable all interrupts
    writeByte(1, 0x00);
    // enable 14 bytes FIFO, DMA mode 0, clear both FIFO queues
    writeByte(2, 0xC7);
    // auxiliary output 2, request to send, data terminal ready
    writeByte(4, 0x0B);
}

}

#endif
