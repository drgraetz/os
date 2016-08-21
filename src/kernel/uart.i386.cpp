#ifdef VERBOSE
#include "kernel.hpp"

namespace uart {

/**
 * The base port of the first serial interface.
 */
#define BASEPORT        0x3F8

void writeByte(unsigned int port, uint8_t value) {
//    print('>');
//    print(' ');
//    printHex(basePort + port);
//    print(',');
//    printHex(value);
//    newLine();
    asm(
        "outb   %%al, %%dx" : :
        "d"(BASEPORT + port), "a"(value)
    );
}

void writeWord(unsigned int port, uint16_t value) {
//    print('>');
//    print(' ');
//    printHex(basePort + port);
//    print(',');
//    printHex(value);
//    newLine();
    asm(
        "outw   %%ax, %%dx" : :
        "d"(BASEPORT + port), "a"(value)
    );
}

uint8_t readByte(unsigned int port) {
    uint8_t result;
    asm(
        "inb    %%dx, %%al" :
        "=a"(result) :
        "d"(BASEPORT + port)
    );
    return result;
}

}

#endif
