#include "kernel.hpp"

tty tty0;

static inline uint8_t readByte(uint16_t port) {
    uint8_t result;
    asm(
        "inb    %%dx"
        : "=a"(result)
        : "d"(port)
    );
    return result;
}

static inline void writeByte(uint16_t port, uint8_t value) {
    asm(
        "outb   %%dx"
        :
        : "d"(port), "a"(value)
    );
}

static inline void writeWord(uint16_t port, uint16_t value) {
    asm(
        "outw   %%dx"
        :
        : "d"(port), "a"(value)
    );
}

// The base port for the com1 interface.
#define COM_BASE                0x3F8

tty::tty() {
    // 8 bits, no parity, 1 stop bit, access divisor latch
    writeByte(COM_BASE + 3, 0x83);
    // speed 9600 baud
    uint16_t divisor = (uint16_t)(115200 / 9600);
    writeWord(COM_BASE + 0, divisor);
    // 8 bits, no parity, 1 stop bit
    writeByte(COM_BASE + 3, 0x03);
    // disable all interrupts
    writeByte(COM_BASE + 1, 0x00);
    // enable 14 bytes FIFO, DMA mode 0, clear both FIFO queues
    writeByte(COM_BASE + 2, 0xC7);
    // auxiliary output 2, request to send, data terminal ready
    writeByte(COM_BASE + 4, 0x0B);
}

ssize_t tty::write(const void* buf, size_t nbyte) {
    if (buf == nullptr || nbyte > SSIZE_MAX) {
        errno = EINVAL;
        return -1;
    }
    const char* current = (const char*)buf;
    ssize_t result = 0;
    while (nbyte--) {
        bool ready = false;
        for (size_t i = 0; !ready && i < 10000; i++) {
            ready = readByte(COM_BASE + 5) & 0x20;
        }
        if (!ready) {
            break;
        }
        writeByte(COM_BASE, (uint8_t)*current);
        current++;
    }
    errno = ESUCCESS;
    return result;
}
