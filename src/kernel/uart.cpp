#ifdef VERBOSE
#include "kernel.hpp"
#include <stdarg.h>

/**
 * The driver for the serial interfaces (COM* on Windows, tty* on Linux). The
 * serial interfaces will only be compiled, if the VERBOSE symbol is defined.
 * They are intended for developing the kernel.
 *
 * A detailed description of how the UART works can be found at
 * http://www.lammertbies.nl/comm/info/serial-uart.html
 *
 * The UART Registers
 * ------------------
 *
 * port        | read                        | write
 * ------------|-----------------------------|--------------------------
 * base + 0    | receive buffer or brd LSB   | transmit buffer or brd LSB
 * base + 1    | interrupt enable or brd MSB | interrupt enable or brd MSB
 * base + 2    | interrupt identification    | FIFO queue control
 * base + 3    | line control                | line control
 * base + 4    | modem control               | modem control
 * base + 5    | line status                 | factory test
 * base + 6    | modem status                | factory test
 *
 * Receive Buffer (read-only) / Transmit Buffer (write-only) (base + 0)
 * --------------------------------------------------------------------
 *
 * The receive (transmit) buffer is used to read (write) a single byte from
 * (to) the serial interface. If the FIFO queue is enabled, the data will be
 * cached in a first-in-first-out buffer. These registers are accessible, if
 * (and only if) bit 7 of the line control register is 0.
 *
 * Baud Rate Divisor (read/write, base + 0...1)
 * --------------------------------------------
 *
 * The baud rate divisor is set to 115200 divided by the baud rate. These
 * registers are accessible, if (and only if) bit 7 of the line control
 * register is 1.
 *
 * Interrupt Enable Register (read/write, base + 1)
 * ------------------------------------------------
 *
 * The UART triggers an IRQ on the processor on certain events, if (and only
 * if) the corresponding bit of the interrupt enable register is set.
 *
 * Bit | Interrupt On
 * ----|-------------------------------------
 * 0   | received data available
 * 1   | transmitter holding register empty
 * 2   | receiver line status register change
 * 3   | modem status register change
 * 4   | sleep mode (16750 only)
 * 5   | low power mode (16750 only)
 *
 * This register is accessible, if (and only if) bit 7 of the line control
 * register is 0.
 *
 * FIFO Queue Control Register (write only, base + 2)
 * --------------------------------------------------
 *
 * Bit(s) | Description
 * -------|-------------------------------------------------------------------
 * 0      | enable FIFO
 * 1      | clear receive FIFO
 * 2      | clear transmit FIFO
 * 3      | 0 = select DMA mode 0, 1 = select DMA mode 1
 * 5      | enable 64 byte FIFO (16750 only)
 * 6,7    | FIFO length (see table below)
 *
 * Bits 6,7 | FIFO length
 * ---------|-------------
 * 0        | 1 byte
 * 1        | 4 bytes
 * 2        | 8 bytes
 * 3        | 14 bytes
 *
 * Interrupt Identification Register (read-only, base + 2)
 * -------------------------------------------------------
 *
 * This register is used to determine, which event triggered an IRQ.
 *
 * Bit(s) | Description
 * -------|---------------------------------------------------
 * 0      | interrupt pending
 * 1-3    | reason for interrupt (see table below for details)
 * 5      | 64 bit FIFO (16750 only)
 * 6-7    | FIFO status (see table below for details)
 *
 * Bits 1-3 | Reason For Interrupt           | Reset By
 * ---------|--------------------------------|--------------------------------
 * 0        | modem status change            | read on modem status
 * 1        | transmitter holding reg. empty | write on transmit buffer
 * 2        | received data available        | read on read buffer
 * 3        | line status change             | read on line status
 * 6        | character timeout (16750 only) | read on read buffer
 *
 * Bits 6-7 | Status of FIFO Queue
 * ---------|----------------------------
 * 0        | no FIFO queue
 * 1        | unusable FIFO (16750 only)
 * 3        | FIFO enabled
 *
 * Line Control Register (read/write, base + 3)
 * --------------------------------------------
 *
 * Bit(s) | Description
 * -------|-------------------------------------------------------------------
 * 0,1    | data word length (see table below for details)
 * 2      | stop bits (see table below for details)
 * 3-5    | parity (see table below for details)
 * 6      | enable break bit
 * 7      | access divisor latch (1) or buffers, and interrupt enable reg. (0)
 *
 * Bits 0,1 | Data Word Length
 * ---------|-----------------
 * 0        | 5 bits
 * 1        | 6 bits
 * 2        | 7 bits
 * 3        | 8 bits
 *
 * Bit 2 | Stop Bits
 * ------|----------------------------------
 * 0     | 1
 * 1     | 1.5 for 5 bit words, 2 otherwise
 *
 * Bits 3-5 | Parity
 * ---------|-------------------------------------------
 * ??0b     | none
 * 001b     | odd
 * 011b     | even
 * 101b     | high (stick)
 * 111b     | low (stick)
 *
 * Bit 7 | Description
 * ------|----------------------------------------------------------
 * 0     | access receiver/transmitter buffer, interrupt enable reg.
 * 1     | access divisor latch
 *
 * Modem Control Register (read/write, base + 4)
 * ---------------------------------------------
 *
 * Bit | Description
 * ----|-------------------------------
 * 0   | data terminal ready
 * 1   | request to send
 * 2   | auxiliary output 1
 * 3   | auxiliary output 2
 * 4   | loopback mode
 * 5   | autoflow control (16750 only)
 *
 * Line Status Register (read-only, base + 5)
 * ------------------------------------------
 *
 * Bit | Description
 * ----|--------------------------------------------------
 * 0   | data available
 * 1   | overrun error
 * 2   | parity error
 * 3   | framing error
 * 4   | break signal received
 * 5   | transmit holding register empty
 * 6   | transmit holding register empty, and line idle
 * 7   | errornous data in FIFO queue
 *
 * Modem Status Register (read-only, base + 6)
 * -------------------------------------------
 *
 * Bit | Description
 * ----|--------------------------------------------------
 * 0   | change in clear to send
 * 1   | change in data set ready
 * 2   | trailing edge ring indicator
 * 3   | change in carrier detect
 * 4   | clear to send
 * 5   | data send ready
 * 6   | ring indicator
 * 7   | carrier detect
 *
 * @author Dr. Florian M. Grätz
 */

namespace uart {

/**
 * Sends a single byte.
 */
void send(char value);
/**
 * Checks, whether the UART is ready for the next character to be sent.
 */
bool isReadyToSend();

}

int putchar(int value) {
    using namespace uart;
    if ((value < 32 || value > 127) && value != '\r' && value != '\n' &&
        value != '\t') {
        value = 127;
    }
    while (!isReadyToSend()) ;
    send((char)value);
    return value;
}

/**
 * Convert an integer number to an 0 terminated ASCII string. If the buffer is
 * not long enough to hold the number, the output is truncated
 * at the buffers end.
 *
 * \return The number of characters written.
*/
uint32_t __itoan(
    char* buffer,           /**< [out]  The buffer, to which the string is
                                        written */
    size_t bufferLength,    /**< [in]   The length of the buffer in bytes */
    int radix,              /**< [in]   The radix of the number system, in
                                        which the number is written */
    unsigned int value,     /**< [in]   The number to be written */
    bool upperCase          /**< [in]   Defines, wether upper (true) or lower
                                        case letters are used for digits
                                        greater than 0 */
) {
    if (buffer == 0 || bufferLength <= 0) {
        return 0;
    }
    int i = 0;
    char digit10 = upperCase ? 'A' : 'a';
    if (bufferLength > 1 && radix > 0 && radix <= 36) {
        for (i = 0; i < bufferLength - 1 && (value != 0 || i == 0); i++) {
            unsigned int newValue = value / radix;
            char digit = (char)(value - newValue * radix);
            if (digit < 10) {
                digit += '0';
            } else {
                digit += digit10 - 10;
            }
            buffer[i] = digit;
            value = newValue;
        }
    }
    buffer[i] = '\0';
    for (char* buffer2 = buffer + i - 1; buffer < buffer2; buffer++, buffer2--) {
        char temp = *buffer2;
        *buffer2 = *buffer;
        *buffer = temp;
    }
    return i;
}

int vprintf(const char* format, va_list arg) {
    if (format == nullptr || !valid(format)) {
        return -1;
    }
    int result = 0;
    char leadingChar;
    int digits;
    const char* string;
    char buffer[33];
    for (char c; (c = *format++) != 0; ) {
        if (c != '%') {
            putchar(c);
            continue;
        }
        leadingChar = ' ';
        digits = 0;
        c = *format++;
        if (c == '0') {
            leadingChar = '0';
            c = *format++;
        }
        while (c >= '0' && c <= '9') {
            digits *= 10;
            digits += c - '0';
            c = *format++;
        }
        switch (c) {
        case 'c':
            buffer[0] = va_arg(arg, unsigned int);
            buffer[1] = 0;
            string = buffer;
            break;
        case 'o':
        case 'O':
            __itoan(buffer, sizeof(buffer), 8, va_arg(arg, unsigned int),
                false);
            string = buffer;
            break;
        case 'd':
        case 'D':
        case 'u':
        case 'U':
            __itoan(buffer, sizeof(buffer), 10, va_arg(arg, unsigned int),
                false);
            string = buffer;
            break;
        case 'x':
            __itoan(buffer, sizeof(buffer), 16, va_arg(arg, unsigned int),
                false);
            string = buffer;
            break;
        case 'X':
            __itoan(buffer, sizeof(buffer), 16, va_arg(arg, unsigned int),
                true);
            string = buffer;
            break;
        case 's':
            string = va_arg(arg, const char*);
            break;
        case 'p':
        case 'P':
            __itoan(buffer, sizeof(buffer), 16, va_arg(arg, unsigned int),
                c == 'P');
            leadingChar = '0';
            digits = 8;
            string = buffer;
            break;
        default:
            putchar(c);
            continue;
        }
        ssize_t len = 0;
        for (const char* tmp = string; *tmp != 0; tmp++) {
            len++;
        }
        for (ssize_t count = digits - len; count > 0; count--) {
            putchar(leadingChar);
        }
        for (; *string != 0; string++) {
            putchar(*string);
        }
    }
    return result;
}

int printf(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    int result = vprintf(format, arg);
    va_end(arg);
    return result;
}

#else

namespace uart {

void init() {
}

}

#endif
