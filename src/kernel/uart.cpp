#ifdef VERBOSE
#include "kernel.hpp"
#include <stdarg.h>

/**
 * @file
 *
 * The architecture-independent implementation of the @ref uart namespace.
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

void assertImpl(bool expression, const char* file, const int line) {
    if (!expression) {
        printf("*** ASSERTION IN %s, line %u ***\r\n", file, line);
        halt();
    }
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
    format = AddressSpace::getPhysicalAddress(format);
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
            string = AddressSpace::getPhysicalAddress(
                va_arg(arg, const char*));
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
