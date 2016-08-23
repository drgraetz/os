#ifndef __KERNEL_HPP
#define __KERNEL_HPP

#include <stdint.h>

/**
 * An architecture specific structure, which is passed to the kernel entry
 * function by the boot loader.
 */
struct boot_data_s;

///**
// * The size of a memory page. The value will be provided by a platform
// * dependend header file.
// */
//#define MEMPAGE_SIZE

/**
 * @file
 * The symbol definitions for the kernel.
 */

///**
// * Yields to the lesser of two values.
// */
//#define min(a, b)           (((a) < (b)) ? (a) : (b))
///**
// * Yields to number of elements in an array.
// */
//#define ARRAYSIZE(a)        (sizeof(a) / sizeof((a)[0]))
//
////extern "C" void* memcpy(void*, const void*, size_t);
extern "C" void* memset(void* ptr, int value, size_t byteCount);

///**
// * Loads an ELF file from a stream.
// */
//errno_e loadElf(Stream& stream);

/**
 * The definition of OS error codes. For a list of the error codes for the most
 * common platforms, refer to <http://www.ioplex.com/~miallen/errcmp.html>
 */
typedef enum {
    ESUCCESS=0,     ///< Operation completed successfully.
    EPERM=1,        ///< Operation not permitted.
    ENOENT=2,       ///< No such file or directory.
    ESRCH=3,        ///< No such process.
    EINTR=4,        ///< Interrupted system call.
    EIO=5,          ///< I/O error.
    ENXIO =6,       ///< No such device or address.
    E2BIG =7,       ///< Argument list too long.
    ENOEXEC=8,      ///< Exec format error.
    EBADF=9,        ///< Bad file number.
    ECHILD=10,      ///< No child process.
    EGAIN =11,      ///< Try again.
    ENOMEM=12,      ///< Not enough space.
    EACCESS=13,     ///< Permission denied.
    EFAULT=14,      ///< Bad address.
    ENOTBLK=15,     ///< Block device required.
    EBUSY =16,      ///< Device or resource busy.
    EEXIST=17,      ///< File exists.
    EXDEV =18,      ///< Cross-device link.
    ENODEV=19,      ///< No such device.
    ENOTDIR=20,     ///< Not a directory.
    EISDIR=21,      ///< Is a directory.
    EINVAL=22,      ///< Invalid argument.
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
 * The errorcode of the last OS operation.
 */
extern errno_e errno;

/**
 * Contains the code for controlling the UART chip. The UART chip is used for
 * debug messages, if compiled with the VERBOSE symbol.
 */
namespace uart {
/**
 * Initializes the UART chip for serial I/O.
 */
void init();
}

#ifdef VERBOSE

int printf(const char*, ...);

#else

#define printf(...)                     {}
#define assert(expression, ...)         {}

#endif

/**
 * Checks wether a pointer is valid. See @ref invalidPtr for the definition
 * of invalid pointers.
 */
static inline bool valid(const void* ptr) {
	return ptr != (const void*)-1;
}

/**
 * The definition of a typed, invalid pointer. Note, that nullptr is a valid
 * pointer to the start of the physical memory.
 */
template<class C> static inline C* invalidPtr() {
	return (C*)-1;
}

/**
 * A virtual address space.
 *
 * i386
 * ====
 *
 * The AddressSpace is represented by a paging directory for the virtual memory
 * management unit. The paging directory points to paging tables. The upper 10
 * bits of an address locate a paging table in the paging directory.
 *
 * If bit 6 of the corresponding page directory entry is 0, the next 10 bits of
 * the address locate a page in the page table. The least significant 12 bits
 * of an address determine an offset within this page.
 *
 * If bit 6 of a page directory entry is 1, it describes a 4 Mbyte page. The
 * least significant 22 bits of an address determine an offset within this
 * page.
 *
 * Each entry of the paging directory or a paging table is built as follows:
 *
 * Bits  | Used In | Description
 * ------|---------|--------------------------------------------------------
 * 12-32 | both    | physical address of the memory page aligned to 4K
 * 9-11  | both    | user flags, can be used by the operating system
 * 8     | table   | global, will not be updated when new dir is loaded
 * 7     | dir     | 4 MByte page is described instead of page table
 * 6     | table   | dirty, set by processor on write access to page
 * 5     | table   | accessed, set by processor on read access to page
 * 4     | both    | cache disabled
 * 3     | both    | write through caching enabled
 * 2     | both    | user (i.e. non ring 0) can access page
 * 1     | both    | write access allowed
 * 0     | both    | present in physical memory
 *
 * Unused bits should remain 0.
 *
 * Global pages will not be updated, if a new paging directory is loaded to
 * CR3. In Grätz.OS it is used for the kernel memory, as it is mapped to the
 * same location in all memory maps.
 */
class AddressSpace {
//	typedef char MemPage[4096];
//    /**
//     * Unmaps a virtual memory block. The memory area does not need to be
//     * mapped.
//     *
//     * @return true on success, false otherwise. If the unmapping failed,
//     * @ref errno is set to an error code.
//     */
//    bool unmap(
//        const void* virtAddr,   ///< The virtual address of the memory block,
//                                ///< which will be unmapped.
//        size_t size             ///< The size of the memory block to be
//                                ///< unmapped. Must be a multiple of the page
//                                ///< size.
//    );
//    /**
//     * Resolves a physical address into a virtual address. If the address has
//     * been mapped multiple times, the last entry is returned.
//     *
//     * @return The requested virtual address or @ref INVALID_PTR, if the
//     * address has not been mapped.
//     */
//    void* getVirtualAddress(
//        const void* physAddr    ///< The physical address, which is resolved.
//    );
//    /**
//     * Loads this into the memory management unit.
//     */
//    void load();
//    /**
//     * Creates a new address space.
//     *
//     * @return The newly created address space or INVALID_PTR, if it could
//     * not be created. In this case, errno holds an error code.
//     */
//    static AddressSpace* create();
//    /**
//     * Maps a virtual memory block to a physical memory block. If the virtual
//     * address has already been mapped, the remaining parameters must match
//     * the already existing mapping.
//     *
//     * @return True on success, false otherwise. If the mapping failed,
//     * @ref errno is set an error code.
//     */
//    bool map(
//        const void* virtAddr,   ///< The virtual address of the memory block.
//                                ///< Must be aligned to a page boundary.
//        const void* physAddr,   ///< The physical address of the memory block.
//                                ///< Must be aligned to a page boundary.
//        size_t size,            ///< The size of the memory block. Must be
//                                ///< a multiple of the page size.
//        bool writable,          ///< Defines, whether write access is allowed.
//        bool userAccess         ///< Defines, whether user code may access the
//                                ///< memory area.
//    );
//    /**
//     * Sets up paging. The following actions are performed:
//     * - The kernel is mapped to its location at the upper boundary of the
//     *   address range.
//     * - The memory management unit is activated.
//     * - The available system memory is reported to be free by invoking
//     *   @ref MemoryManager::markAsFree(uint32_t)
//     */
//    static void init(
//    );
private:
    /**
     * The type independent implementation of @ref getPhysicalAddress
     */
    static void* getPhysicalAddressImpl(
        const void* virtAddr
    );
public:
    /**
     * The kernel's address space.
     */
    static AddressSpace kernel;
#ifdef VERBOSE
    /**
     * Prints the contents of the paging tables. For each area of virtual
     * memory with the same attributes a line is printed. It contains the
     * following information:
     * - the virtual start and end addresses
     * - the physical start address
     * - the attributes of the memory block
     * - the user flags, which are one of the following: Kernel, User, Boot
     * The attributes are abbreviated with a single letter, as they are defined
     * in the following table.
     *
     * Letter | Value
     * -------|----------------------------------
     * G      | @ref PA_GLOBAL
     * L      | @ref PA_4MBYTE
     * D      | @ref PA_DIRTY
     * A      | @ref PA_ACCESSED
     * C      | not @ref PA_NOCACHE, i.e. cached
     * T      | @ref PA_WRITETHRU
     * U      | @ref PA_RING0, K otherwise
     * W      | @ref PA_WRITABLE, R otherwise
     * P      | @ref PA_PRESENT
     */
    void dump();
#endif
    /**
     * Resolves a virtual address into a physical address.
     *
     * @return The requested physical address or @ref INVALID_PTR, if the
     * address has not been mapped to physical memory.
     */
    template<class C> static inline C* getPhysicalAddress(
        const C* virtAddr   ///< The virtual address, which will be translated.
    ) { return (C*)getPhysicalAddressImpl(virtAddr); }
};

///**
// * The kernel's memory management functionality.
// */
//class MemoryManager {
//public:
//    /**
//     * Marks a memory page as being free. This method should only be invoked
//     * during the initialization phase of the kernel, i.e. prior to a call of
//     * @ref kmain()
//     */
//    static void markAsFree(
//        int phys 			///< The index of the next free memory page. Using
//                            ///< indexing instead of actual addresses, the
//                            ///< address range of available memory pages is
//                            ///< significantly increased, which may come in
//                            ///< handy on some later 32 bit intel machines.
//    );
//    /**
//     * Returns the number of free memory pages.
//     */
//    static uint32_t getFreePagesCount();
//    /**
//     * Allocates a new page of memory.
//     *
//     * @return the index of the newly allocated memory page or -1, if no free
//     * memory is available.
//     */
//    static int allocate(
//		bool allowHighMemory	///< If false, only memory below the kernel's
//								///< virtual memory address is allowed.
//	);
//};

/**
 * The entry function to the kernel. This function is implemented in the file
 * boot.*.S, where * stands for the target platform. The entry function
 * performs the following operations:
 * - validate, that the boot process has been completed successfully
 * - map the kernel into its virtual memory location
 * - initialize the kernel's stack
 * - invoke @ref kmain(struct boot_data_s&)
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
extern "C" void kmain(
    struct boot_data_s& data    ///< Pointer to an architecture specific data
                                ///< structure, which is passed by the boot
                                ///< loader.
) __attribute__((noreturn));

#endif
