#ifndef __KERNEL_HPP
#define __KERNEL_HPP

#include <stdint.h>

/**
 * An architecture specific structure, which is passed to the kernel entry
 * function by the boot loader.
 */
struct boot_data_s;

/**
 * @file
 * The symbol definitions for the kernel.
 *
 * @author Dr. Florian Manfred Grätz
 */

/**
 * The definition of a publicly visible declaration, accessible from C code.
 */
#define CDECL   extern "C"

///**
// * Yields to the lesser of two values.
// */
//#define min(a, b)           (((a) < (b)) ? (a) : (b))
/**
 * Yields to number of elements in an array.
 */
template <typename T, size_t N> constexpr size_t arraySize(const T (&)[N]) {
    return N;
}
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

void assertImpl(bool expression, const char* file, const int line);
int printf(const char*, ...);
#define assert(expression)  { assertImpl(expression, __FILE__, __LINE__); }

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
//     * Creates a new address space.
//     *
//     * @return The newly created address space or INVALID_PTR, if it could
//     * not be created. In this case, errno holds an error code.
//     */
//    static AddressSpace* create();
private:
    /**
     * Loads this into the memory management unit.
     */
    void load();
    /**
     * The first byte of the kernel in the physical memory. The address is
     * defined in the linker script and its value is derived from
     * buildinfo.xml.
     */
    static char PHYSICAL_ADDR;
    /**
     * The first byte of the kernel in the virtual memory. The address is
     * defined in the linker script and its value is derived from
     * buildinfo.xml.
     */
    static char KERNEL_CODE;
    /**
     * The first byte of read-only data in the virtual memory. The address is
     * defined in the linker script.
     */
    static char KERNEL_READ_ONLY;
    /**
     * The first byte of read-write data in the virtual memory. The address is
     * defined in the linker script.
     */
    static char KERNEL_READ_WRITE;
    /**
     * The last byte the kernel in the virtual memory. The address is defined
     * in the linker script.
     */
    static char KERNEL_END;
    /**
     * The end of the stack. The address is defined in the boot.*.S file.
     */
public:
    static char STACK;
private:
    /**
     * The type independent implementation of @ref getPhysicalAddress
     */
    static void* getPhysicalAddressImpl(
        const void* virtAddr
    );
    /**
     * Adjust the addresses of the second level tables. This is required for
     * the @ref kernel address space, as for it tables are defined in the
     * virtual address space of the kernel.
     */
    void adjustTableAddresses();
    /**
     * Maps the kernel to a virtual address.
     */
    void mapKernel(
        const void* virtAddr    ///< The virtual address, the kernel is mapped
                                ///< to.
    );
public:
    /**
     * @return the size of a memory page.
     */
    static size_t getPageSize();
    /**
     * Maps a virtual memory block to a physical memory block. If the virtual
     * address has already been mapped, the attributes of the mapped block are
     * adjusted. If the physical address lies within the virtual address range
     * of the kernel, the mapping is marked as global.
     */
    void map(
        const void* virtAddr,   ///< The virtual address of the memory block.
                                ///< Must be aligned to a page boundary.
        const void* physAddr,   ///< The physical address of the memory block.
                                ///< Must be aligned to a page boundary.
        size_t size,            ///< The size of the memory block. Must be
                                ///< a multiple of the page size.
        bool writable,          ///< Defines, whether write access is allowed.
        bool userAccess         ///< Defines, whether user code may access the
                                ///< memory area.
    );
    /**
     * Sets up paging. The following actions are performed:
     * - The kernel is mapped 1:1 to its physical address. This is necessary,
     *   as otherwise the code would continue execution at an invalid address
     *   immediately after the activation of the memory management unit.
     * - The kernel is mapped to its virtual location at the upper boundary of
     *   the address range.
     * - The memory management unit is activated.
     */
    static void init();
    /**
     * Enables paging. Therefore, the following steps are performed:
     * - the the memory management unit is activated
     * - the program counter register is updated to the virtual memory location
     *   of the kernel
     * - the stack pointers are updated to the virtual memory location of the
     *   kernel
     * - all pointers residing on the stack are adjusted
     *
     * Prior to enablePaging() @ref mapKernel(const void*) has to be invoked.
     * Otherwise the processor will cause an page fault, as program execution
     * will be continued at a virtual address that is not mapped to the
     * physical memory.
     */
    static void enablePaging();
    /**
     * The kernel's address space.
     */
    static AddressSpace kernel;
#ifdef VERBOSE
    /**
     * Prints the contents of the paging tables. For each area of virtual
     * memory with the same attributes a line is printed with the following
     * information:
     * - the virtual start and end addresses
     * - the physical start address
     * - the platform specific attributes of the memory block
     *
     * i386
     * ====
     *
     * Letter | Value
     * -------|----------------------------------------------------
     * G      | globally accessible
     * D      | dirty, i.e. the processor has written to the page
     * A      | accessed, i.e. the processor has read from the pae
     * C      | cached
     * T      | write-through-caching
     * U/K    | user/kernel mode
     * W/R    | writable/read-only
     * P      | present in memory
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
 * - initialize the kernel's stack
 * - invoke @ref kmain(struct boot_data_s&)
 */
CDECL void _start() __attribute__((noreturn));

/**
 * Halts the execution of code on the currently active CPU. This function is
 * implemented in the file boot.*.S, where * stands for the target platform.
 */
CDECL void halt() __attribute__((noreturn));

/**
 * The kerneml's main function.
 */
CDECL void kmain(
    struct boot_data_s& data    ///< Pointer to an architecture specific data
                                ///< structure, which is passed by the boot
                                ///< loader.
) __attribute__((noreturn));

#endif
