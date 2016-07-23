#ifndef __KERNEL_HPP
#define __KERNEL_HPP

#include <stdint.h>

/**
 * @file
 * The symbol definitions for the kernel.
 */

extern "C" void exc0() __attribute__((noreturn));

void* memset(void* ptr, int value, size_t byteCount);

int printf(const char*, ...);
int putchar(int);
#define assert(expression, __VA_ARGS__) {   \
    if (!(expression)) {                    \
        printf(__VA_ARGS__);                \
        halt();                             \
    } }

/**
 * The definition of OS error codes. For a list of the error codes for the most
 * common platforms, refer to <http://www.ioplex.com/~miallen/errcmp.html>
 */
typedef enum {
    ESUCCESS = 0,   ///< Operation completed successfully.
    EPERM = 1,      ///< Operation not permitted.
    ENOENT = 2,     ///< No such file or directory.
    ESRCH = 3,      ///< No such process.
    EINTR = 4,      ///< Interrupted system call.
    EIO = 5,        ///< I/O error.
    ENXIO = 6,      ///< No such device or address.
    E2BIG = 7,      ///< Argument list too long.
    ENOEXEC = 8,    ///< Exec format error.
    EBADF = 9,      ///< Bad file number.
    ECHILD = 10,    ///< No child process.
    EGAIN = 11,     ///< Try again.
    ENOMEM = 12,    ///< Not enough space.
    EACCESS = 13,   ///< Permission denied.
    EFAULT = 14,    ///< Bad address.
    ENOTBLK = 15,   ///< Block device required.
    EBUSY = 16,     ///< Device or resource busy.
    EEXIST = 17,    ///< File exists.
    EXDEV = 18,     ///< Cross-device link.
    ENODEV = 19,    ///< No such device.
    ENOTDIR = 20,   ///< Not a directory.
    EISDIR = 21,    ///< Is a directory.
    EINVAL = 22,    ///< Invalid argument.
    ENFILE=23,      ///< File table overflow.
    EMFILE=24,      ///< Too many open files.
    ENOTTY=25,      ///< Not a typewriter.
    ETXTBSY=26      ///< Text file busy.
} errno_e;

/**
 * The errorcode of the last OS operation.
 */
extern errno_e errno;

/**
 * The driver for the serial UART ports, available as tty devices to posix
 * compatible kernels.
 */
class tty {
public:
    tty();
    /**
     * The implimentation of a posix write command. Note, thatthe file handle
     * is made obsolete by the this pointer.
     *
     * For the posix specification of the write function, refer to
     * <http://pubs.opengroup.org/onlinepubs/009695399/functions/write.html>
     */
    ssize_t write(const void* buf, size_t nbyte);
};

/**
 * The tty device (required for debugging).
 */
extern tty tty0;

/**
 * The entry function to the kernel. This function is implemented in the file
 * boot.*.S, where * stands for the target platform. The entry function
 * performs the following operations:
 * - validate, that the boot process has been completed successfully
 * - enable the memory management unit
 * - make the kernel accessable in the highest MByte of virtual memory
 * - set the access rights to the kernel memory such that the data and stack
 *   are inaccessible from user code and the read-only data cannot be written
 * - invoke @ref kmain(void)
 * - if kmain returns (which is not expected to happen): hang the processor
 *   infinetly
 */
extern "C" void _start() __attribute__((noreturn));

/**
 * Halts the execution of code on the currently active CPU. This function is
 * implemented in the file boot.*.S, where * stands for the target platform.
 */
extern "C" void halt() __attribute__((noreturn));

/**
 * The kerneml's main function.
 */
extern "C" void kmain() __attribute__((noreturn));

#endif
