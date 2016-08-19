//#include "kernel.hpp"
//
//#ifdef VERBOSE
//
///**
// * The driver for the serial interfaces (COM* on Windows, tty* on Linux).
// *
// * A detailed description of how the UART works can be found at
// * http://www.lammertbies.nl/comm/info/serial-uart.html
// *
// * The UART Registers
// * ------------------
// *
// * port        | read                        | write
// * ------------|-----------------------------|--------------------------
// * base + 0    | receive buffer or brd LSB   | transmit buffer or brd LSB
// * base + 1    | interrupt enable or brd MSB | interrupt enable or brd MSB
// * base + 2    | interrupt identification    | FIFO queue control
// * base + 3    | line control                | line control
// * base + 4    | modem control               | modem control
// * base + 5    | line status                 | factory test
// * base + 6    | modem status                | factory test
// *
// * Receive Buffer (read-only) / Transmit Buffer (write-only) (base + 0)
// * --------------------------------------------------------------------
// *
// * The receive (transmit) buffer is used to read (write) a single byte from
// * (to) the serial interface. If the FIFO queue is enabled, the data will be
// * cached in a first-in-first-out buffer. These registers are accessible, if
// * (and only if) bit 7 of the line control register is 0.
// *
// * Baud Rate Divisor (read/write, base + 0...1)
// * --------------------------------------------
// *
// * The baud rate divisor is set to 115200 divided by the baud rate. These
// * registers are accessible, if (and only if) bit 7 of the line control
// * register is 1.
// *
// * Interrupt Enable Register (read/write, base + 1)
// * ------------------------------------------------
// *
// * The UART triggers an IRQ on the processor on certain events, if (and only
// * if) the corresponding bit of the interrupt enable register is set.
// *
// * Bit | Interrupt On
// * ----|-------------------------------------
// * 0   | received data available
// * 1   | transmitter holding register empty
// * 2   | receiver line status register change
// * 3   | modem status register change
// * 4   | sleep mode (16750 only)
// * 5   | low power mode (16750 only)
// *
// * This register is accessible, if (and only if) bit 7 of the line control
// * register is 0.
// *
// * FIFO Queue Control Register (write only, base + 2)
// * --------------------------------------------------
// *
// * Bit(s) | Description
// * -------|-------------------------------------------------------------------
// * 0      | enable FIFO
// * 1      | clear receive FIFO
// * 2      | clear transmit FIFO
// * 3      | 0 = select DMA mode 0, 1 = select DMA mode 1
// * 5      | enable 64 byte FIFO (16750 only)
// * 6,7    | FIFO length (see table below)
// *
// * Bits 6,7 | FIFO length
// * ---------|-------------
// * 0        | 1 byte
// * 1        | 4 bytes
// * 2        | 8 bytes
// * 3        | 14 bytes
// *
// * Interrupt Identification Register (read-only, base + 2)
// * -------------------------------------------------------
// *
// * This register is used to determine, which event triggered an IRQ.
// *
// * Bit(s) | Description
// * -------|---------------------------------------------------
// * 0      | interrupt pending
// * 1-3    | reason for interrupt (see table below for details)
// * 5      | 64 bit FIFO (16750 only)
// * 6-7    | FIFO status (see table below for details)
// *
// * Bits 1-3 | Reason For Interrupt           | Reset By
// * ---------|--------------------------------|--------------------------------
// * 0        | modem status change            | read on modem status
// * 1        | transmitter holding reg. empty | write on transmit buffer
// * 2        | received data available        | read on read buffer
// * 3        | line status change             | read on line status
// * 6        | character timeout (16750 only) | read on read buffer
// *
// * Bits 6-7 | Status of FIFO Queue
// * ---------|----------------------------
// * 0        | no FIFO queue
// * 1        | unusable FIFO (16750 only)
// * 3        | FIFO enabled
// *
// * Line Control Register (read/write, base + 3)
// * --------------------------------------------
// *
// * Bit(s) | Description
// * -------|-------------------------------------------------------------------
// * 0,1    | data word length (see table below for details)
// * 2      | stop bits (see table below for details)
// * 3-5    | parity (see table below for details)
// * 6      | enable break bit
// * 7      | access divisor latch (1) or buffers, and interrupt enable reg. (0)
// *
// * Bits 0,1 | Data Word Length
// * ---------|-----------------
// * 0        | 5 bits
// * 1        | 6 bits
// * 2        | 7 bits
// * 3        | 8 bits
// *
// * Bit 2 | Stop Bits
// * ------|----------------------------------
// * 0     | 1
// * 1     | 1.5 for 5 bit words, 2 otherwise
// *
// * Bits 3-5 | Parity
// * ---------|-------------------------------------------
// * ??0b     | none
// * 001b     | odd
// * 011b     | even
// * 101b     | high (stick)
// * 111b     | low (stick)
// *
// * Bit 7 | Description
// * ------|----------------------------------------------------------
// * 0     | access receiver/transmitter buffer, interrupt enable reg.
// * 1     | access divisor latch
// *
// * Modem Control Register (read/write, base + 4)
// * ---------------------------------------------
// *
// * Bit | Description
// * ----|-------------------------------
// * 0   | data terminal ready
// * 1   | request to send
// * 2   | auxiliary output 1
// * 3   | auxiliary output 2
// * 4   | loopback mode
// * 5   | autoflow control (16750 only)
// *
// * Line Status Register (read-only, base + 5)
// * ------------------------------------------
// *
// * Bit | Description
// * ----|--------------------------------------------------
// * 0   | data available
// * 1   | overrun error
// * 2   | parity error
// * 3   | framing error
// * 4   | break signal received
// * 5   | transmit holding register empty
// * 6   | transmit holding register empty, and line idle
// * 7   | errornous data in FIFO queue
// *
// * Modem Status Register (read-only, base + 6)
// * -------------------------------------------
// *
// * Bit | Description
// * ----|--------------------------------------------------
// * 0   | change in clear to send
// * 1   | change in data set ready
// * 2   | trailing edge ring indicator
// * 3   | change in carrier detect
// * 4   | clear to send
// * 5   | data send ready
// * 6   | ring indicator
// * 7   | carrier detect
// *
// */
//
//tty tty0;
//
//static inline uint8_t readByte(uint16_t port) {
//    uint8_t result;
//    asm(
//        "inb    %%dx"
//        : "=a"(result)
//        : "d"(port)
//    );
//    return result;
//}
//
//static inline void writeByte(uint16_t port, uint8_t value) {
//    asm(
//        "outb   %%dx"
//        :
//        : "d"(port), "a"(value)
//    );
//}
//
//static inline void writeWord(uint16_t port, uint16_t value) {
//    asm(
//        "outw   %%dx"
//        :
//        : "d"(port), "a"(value)
//    );
//}
//
//// The base port for the com1 interface.
//#define COM_BASE                0x3F8
//
//tty::tty() {
//    // 8 bits, no parity, 1 stop bit, access divisor latch
//    writeByte(COM_BASE + 3, 0x83);
//    // speed 9600 baud
//    uint16_t divisor = (uint16_t)(115200 / 9600);
//    writeWord(COM_BASE + 0, divisor);
//    // 8 bits, no parity, 1 stop bit
//    writeByte(COM_BASE + 3, 0x03);
//    // disable all interrupts
//    writeByte(COM_BASE + 1, 0x00);
//    // enable 14 bytes FIFO, DMA mode 0, clear both FIFO queues
//    writeByte(COM_BASE + 2, 0xC7);
//    // auxiliary output 2, request to send, data terminal ready
//    writeByte(COM_BASE + 4, 0x0B);
//}
//
//off_t tty::seek(off_t pos) {
//    return EIO;
//}
//
//ssize_t tty::read(void* buf, size_t nbyte) {
//    if (buf == nullptr || nbyte > SSIZE_MAX) {
//        errno = EINVAL;
//        return -1;
//    }
//    errno = EPERM;
//    return -1;
//}
//
//ssize_t tty::write(const void* buf, size_t nbyte) {
//    if (buf == nullptr || nbyte > SSIZE_MAX) {
//        errno = EINVAL;
//        return -1;
//    }
//    const char* current = (const char*)buf;
//    ssize_t result = 0;
//    while (nbyte--) {
//        bool ready = false;
//        for (size_t i = 0; !ready && i < 10000; i++) {
//            ready = readByte(COM_BASE + 5) & 0x20;
//        }
//        if (!ready) {
//            break;
//        }
//        writeByte(COM_BASE, (uint8_t)*current);
//        current++;
//    }
//    errno = ESUCCESS;
//    return result;
//}
//
//tty tty::tty0;
//
//#endif
