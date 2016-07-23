#ifndef __KERNEL_HPP
#define __KERNEL_HPP

#include <stdint.h>

/**
 * @file
 * The symbol definitions for the kernel.
 */

 
 /**
  * The definition of an invalid pointer. Note, that nullptr is a valid
  * pointer to the start of the physical memory.
  */
 #define INVALID_PTR        ((void*)(-1))
 
/**
 * The interface for streaming data. This corresponds to a posix file handle.
 */
class Stream {
public:
    /**
     * The implementation of a posix lseek command. The whence parameter is
     * set to SEEK_SET.
     */
    virtual off_t seek(off_t offset) = 0;
    /**
     * The implimentation of a posix write command.
     *
     * For the posix specification of the write function, refer to
     * <http://pubs.opengroup.org/onlinepubs/009695399/functions/write.html>
     */
    virtual ssize_t write(const void* buf, size_t nbyte) = 0;
    /**
     * The implementation of a posix write command.
     *
     * For the posix specification of the write function, refer to
     * <http://pubs.opengroup.org/onlinepubs/009695399/functions/read.html>
     */
    virtual ssize_t read(void* buf, size_t nbyte) = 0;
};

/**
 * A stream writing to / reading from a memory area.
 */
class MemoryStream : public Stream {
private:
    char* start;
    char* end;
    char* current;
public:
    /**
     * Creates a new instance working on a buffer.
     */
    MemoryStream(
        void* buffer,   ///< The buffer to work on.
        size_t size,    ///< The size of the buffer in bytes.
        size_t pos      ///< The position in the buffer for the next read/write
                        ///< operation.
    );
    virtual off_t seek(off_t offset);
    virtual ssize_t write(const void* buf, size_t nbyte);
    virtual ssize_t read(void* buf, size_t nbyte);
};

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
    ETXTBSY=26,     ///< Text file busy.
    EFBIG=27,       ///< File too large.
    ENOSPC=28,      ///< No space left on device.
    ESPIPE=29,      ///< Illegal seek.
    EROFS=30,       ///< Read only file system.
    EMLINK=31,      ///< Too many links.
    EPIPE=32,       ///< Broken pipe.
    EDOM=33,        ///< Math arg out of domain of func.
    ERANGE=34,      ///< Math result not representable.
    ENOMSG=35,      ///< No message of desired type.
    EEIDRM=36,      ///< Identifier removed.
    ECHRNG=37,      ///< Channel number out of range.
    EL2NSYNC=38,    ///< Level 2 not synchronized.
    EL3HLT=39,      ///< Level 3 halted.
    EL3RST=40,      ///< Level 3 reset.
    ELNRNG=41,      ///< Link number out of range.
    EUNATCH=42,     ///< Protocol driver not attached.
    ENOCSI=43,      ///< No CSI structure available.
    EL2HLT=44,      ///< Level 2 halted.
    EDEADLK=45,     ///< Deadlock condition.
    ENOTREADY=46,   ///< Device not ready.
    EWRPROTECT=47,  ///< Write protected media.
    EFORMAT=48,     ///< Unformatted media.
} errno_e;

/**
 * Loads an ELF file from a stream.
 */
errno_e loadElf(Stream& stream);

/**
 * The errorcode of the last OS operation.
 */
extern errno_e errno;

/**
 * The driver for the serial UART ports, available as tty devices to posix
 * compatible kernels.
 */
class tty : public Stream {
public:
    tty();
    virtual off_t seek(off_t offset);
    virtual ssize_t write(const void* buf, size_t nbyte);
    virtual ssize_t read(void* buf, size_t nbyte);
};

/**
 * A virtual address space.
 */
class AddressSpace {
protected:
    /**
     * The kernel's address space.
     */
    static AddressSpace kernel;
public:
    /**
     * Sets up paging. The following actions are performed:
     * - The pointers to the paging tables in the @ref AddressSpace::kernel
     *   are adjusted to physical addresses.
     * - The kernel is mapped 1:1 to its physical memory, so the physical
     *   memory is marked as used.
     * - The kernel is mapped to its location at the upper boundary of the
     *   address range. The corresponding paging table is marked as global.
     *   This is possible, as all code will access the kernel at this address.
     *   Marking the memory area as global will speed up access to it, as the
     *   paging tables are not updated on task switches.
     * - The memory management unit is activated.
     */
    static void init();
    /**
     * Creates a new address space.
     *
     * @return The newly created address space or INVALID_PTR, if it could
     * not be created. In this case, errno holds an error code.
     */
    static AddressSpace* create();
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
