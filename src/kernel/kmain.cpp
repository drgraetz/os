#include "kernel.hpp"
#include <stdarg.h>

/**
 * @file
 *
 * Holds the kernel's main function.
 */

void* operator new (unsigned int size, void* ptr) noexcept;

int putchar(int value) {
    if ((value < 32 || value > 127) && value != '\r' && value != '\n' &&
        value != '\t') {
        value = 127;
    }
    tty0.write(&value, 1);
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
    uint32_t bufferLength,  /**< [in]   The length of the buffer in bytes */
    uint32_t radix,         /**< [in]   The radix of the number system, in
                                        which the number is written */
    uint32_t value,         /**< [in]   The number to be written */
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

extern const char CODE;
extern const char PHYS;

int vprintf(const char* format, va_list arg) {
    if (format == nullptr) {
        return -1;
    }
    format -= (&CODE - &PHYS);
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
            if (string >= &CODE) {
                string -= (&CODE - &PHYS);
            }
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

errno_e errno = ESUCCESS;

/**
 * The start of the list of module initializers. See @ref initModules() for
 * details. This symbols is defined by the linker script.
 */
extern "C" void(*__CTOR_LIST__)();
/**
 * The end of the list of module initializers. See @ref initModules() for
 * details. This symbol is defined by the linker script.
 */
extern "C" void(*__CTOR_END__)();

/**
 * Invokes all module initializers. This step is necessary, as no C runtime
 * library is available for the kernel. The linker scripts defines the start
 * and the end of the list of initializers. The list of initializers is an
 * array of function pointers. The initalizer functions are parameterless and
 * do not provide return values.
 */
void initModules() {
    for (void (**init)() = &__CTOR_LIST__; init < &__CTOR_END__; init++) {
        if (*init != nullptr) {
            (*init)();
        }
    }
}

void kmain() {
    initModules();
    printf("Hello from the kernel.\r\n");
    halt();
}
