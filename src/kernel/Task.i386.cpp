#include <multiboot/multiboot.h>
#include "kernel.hpp"

#define PAGESIZE        4096
#define TRUNC(x)        ((uint32_t)(x) & ~(PAGESIZE - 1))
#define ISFREE(entry)   (((entry) & PA_USERMASK) == PA_FREE)
#define ATTR(entry)     (((uint32_t)entry) & (PAGESIZE - 1))
/**
 * The start of the kernel code in the physical address range.
 */
extern const char PHYS;
/**
 * The start of the kernel code in the virtual address range.
 */
extern const char CODE;
/**
 * The start of the data area in the virtual address range.
 */
extern const char READWRITE;
/**
 * The start of the constant data area in the virtual address range.
 */
extern const char READONLY;
/**
 * The end of the kernel in the virtual address range.
 */
extern const char END;

namespace i386 {

/**
 * A paging directory for the virtual memory management unit. The page
 * directory points to page tables. The upper 10 bits of an address locate a
 * page table in the page directory.
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
class PagingDirectory {
private:
    /**
     * The attribute definition for a memory page.
     */
    typedef enum {
        PA_PRESENT = 1,     ///< Physically present in memory.
        PA_WRITABLE = 2,    ///< Write access allowed.
        PA_RING0 = 4,       ///< Accessible by user code.
        PA_WRITETHRU = 8,   ///< Write through caching enabled.
        PA_NOCACHE = 16,    ///< Caching disabled.
        PA_ACCESSED = 32,   ///< Set by read operation on memory in page.
        PA_DIRTY = 64,      ///< Set by write operation on memory in page.
        PA_4MBYTE = 128,    ///< Directory entry discrables 4Mbyte page instead
                            ///< of page table.
        PA_GLOBAL = 256,    ///< Entry is used globally, i.e. it is not
                            ///< updated, when a new table is loaded.
        PA_USERMASK = 0xE00,///< Bitmask for singling out user bits.
        PA_FREE = 0x000,    ///< Page is free.
        PA_USER = 0x200,    ///< Page contains user code.
        PA_BOOT = 0x400,    ///< Page contains bootstrap code or BIOS data.
        PA_KERNEL = 0xE00,  ///< Page contains kernel code.
    } attr_e;
    /**
     * A paging table or paging directory. The upper 20 bits of every entry
     * point to an address, the lower 12 bits define a memory page's attribute.
     * @ref attr_e for a detailed description of possible page attributes.
     */
    typedef uint32_t pageTable_t[1024] __attribute__((aligned(4096)));
    /**
     * The contents of this. The topmost 10 bits of every pointer select an
     * entry in this. If the @ref PA_4MBYTE bit is set, the lower 22 bits
     * select an entry in the 4 MByte page pointed to by the directory entry.
     * Otherwise, the bits 12 ... 21 select an entry in the @ref pageTable_t
     * pointed to by the entry in the directory. In the latter case, the lowest
     * 12 bits define an offset in the 4 KByte page pointed to by the page
     * table.
     */
    pageTable_t data;
    /**
     * Allocates a single page of memory.
     * @return The physical address of the newly allocated memory page.
     */
    static void* allocate() {
        printf("**** FUNCTION NOT IMPLEMENTED ****\r\n");
        printf("PageTable::allocate()\r\n");
        halt();
    }
    
    /**
     * Merges a set of given attributes into an entry for a paging table or
     * a paging directory. The rules, which apply, are described for
     * @ref map(const void*, const void*, size_t, attr_e)
     */
    static void merge(
        uint32_t* entry,    ///< A pointer to the table/directory entry, which
                            ///< is merged.
        attr_e attribute    ///< The attributes, which are merged with the
                            ///< entry.
    ) {
        uint32_t value = *entry;
        if ((attribute & PA_USERMASK) > (value & PA_USERMASK)) {
            value = (value & (~PA_USERMASK)) | (attribute & PA_USERMASK);
        }
        value |= (attribute & (PA_PRESENT | PA_WRITABLE | PA_RING0 |
            PA_ACCESSED | PA_DIRTY));
        uint32_t andmask = PA_WRITETHRU | PA_NOCACHE;
        value = (value & ~andmask) | (value & attribute & andmask);
        *entry = value;
    }
    
    /**
     * Maps a block of memory from a physical address to a virtual address. If
     * one of the memory pages, which will be mapped, overlaps with an already
     * mapped memory page, the attributes of the physical memory page are
     * merged as follows:
     * - the flags PA_PRESENT, PA_WRITABLE, PA_RING0 are ored
     * - the flags PA_WRITETHRU, PA_NOCACHE are anded
     * - the user flags are set to the greater of the two values
     */
    void map(
        const void* physAddr,   ///< The start address of the memory block,
                                ///< which should be mapped in physical memory.
                                ///< It does not need to be aligned with a
                                ///< memory page. In this case, the entire
                                ///< memory page is mapped.
        const void* virtAddr,   ///< The start address in virtual memory. It
                                ///< must have the same offset from the start
                                ///< of the memory page like the physical
                                ///< address.
        size_t size,            ///< The number of bytes, which will be mapped.
                                ///< Neither the physical nor the virtual
                                ///< address must overrun the 0 address.
        attr_e attribute        ///< The attributes, with which the memory
                                ///< should be marked. The flags PA_ACCESSED,
                                ///< PA_DIRTY, and PA_4MBYTE should not be
                                ///< used. The user flags must be different
                                ///< from PA_FREE.
    ) {
        assert(ATTR(virtAddr) == ATTR(physAddr),
            "PagingDirectory::map(): invalid page alignment.");
        assert((attribute & PA_4MBYTE) == 0,
            "PagingDirectory::map(): 4 MByte page are not supported.");
        assert((uint32_t)physAddr + size > (uint32_t)physAddr ||
            (uint32_t)virtAddr + size > (uint32_t)virtAddr,
            "PagingDirectory::map(): address overflow.");
        assert(!ISFREE(attribute),
            "PagingDirectory::map(): cannot map free memory.");
        size += ATTR(physAddr);
        physAddr = (const void*)TRUNC(physAddr);
        virtAddr = (const void*)TRUNC(virtAddr);
        for (size += PAGESIZE; size > PAGESIZE; size -= PAGESIZE) {
            uint32_t* dirEntry = getDirEntry(virtAddr);
            if (ISFREE(*dirEntry)) {
                *dirEntry = (uint32_t)allocate() | attribute;
            } else {
                merge(dirEntry, attribute);
            }
            uint32_t* tableEntry = getTableEntry(virtAddr);
            if (ISFREE(*tableEntry)) {
                *tableEntry = (uint32_t)physAddr | attribute;
            } else {
                assert(TRUNC(*tableEntry) == TRUNC(physAddr),
                   "PagingDirectory::map():: cannot relocate memory.");
                merge(tableEntry, attribute);
            }
            physAddr = (const void*)((uint32_t)physAddr + PAGESIZE);
            virtAddr = (const void*)((uint32_t)virtAddr + PAGESIZE);
        }
    }
    /**
     * Gets a pointer to the entry in the paging directory for a virtual
     * address.
     *
     * @return a pointer to the entry in the paging directory.
     */
    uint32_t* getDirEntry(
        const void* virtAddr    ///< The virtual address, for which the paging
                                ///< directory entry is saught.
    ) {
        return data + ((uint32_t)virtAddr >> 22);
    }
    /**
     * Gets a pointer to the entry in the paging table for a virtual address.
     * Asserts, if no paging table exists.
     *
     * @return a pointer to the entry in the paging table.
     */
    uint32_t* getTableEntry(
        const void* virtAddr    ///< The virtual address, for which the paging
                                ///< directory entry is saught.
    ) {
        uint32_t dirEntry = *getDirEntry(virtAddr);
        assert(!ISFREE(dirEntry),
            "PagingDirectory::getTableEntry(): no paging table associated.");
        return (uint32_t*)TRUNC(dirEntry) +
            (((uint32_t)virtAddr >> 12) & 1023);
    }
    /**
     * Gets the value of an entry in the paging table for a virtual address.
     *
     * @return the requested value, or 0 if no paging table exists for the
     * requested virtual address.
     */
    uint32_t getEntry(
        const void* virtAddr    ///< The virtual address, for which the paging
                                ///< directory entry is saught.
    ) {
        uint32_t* dirEntry = getDirEntry(virtAddr);
        if (ISFREE(*dirEntry)) {
            return 0;
        } else {
            return *getTableEntry(virtAddr);
        }
    }
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
    void dump() {
        printf("===================================================\r\n");
        printf("PagingDirectory @ %p\r\n", this);
        const char* start = nullptr;
        do {
            uint32_t startEntry = getEntry(start);
            if (!ISFREE(startEntry)) {
                const char* end = start;
                uint32_t endEntry;
                uint32_t startAttr = ATTR(startEntry);
                do {
                    end += PAGESIZE;
                    endEntry = getEntry(end);
                } while (end != nullptr && startAttr == ATTR(endEntry));
                char attribs[10];
                char* current = attribs;
                *current++ = startAttr & PA_GLOBAL ? 'G' : '-';
                *current++ = startAttr & PA_4MBYTE ? 'L' : '-';
                *current++ = startAttr & PA_DIRTY ? 'D' : '-';
                *current++ = startAttr & PA_ACCESSED ? 'A' : '-';
                *current++ = startAttr & PA_NOCACHE ? '-' : 'C';
                *current++ = startAttr & PA_WRITETHRU ? 'T' : '-';
                *current++ = startAttr & PA_RING0 ? 'U' : 'K';
                *current++ = startAttr & PA_WRITABLE ? 'W' : 'R';
                *current++ = startAttr & PA_PRESENT ? 'P' : '-';
                *current = 0;
                const char* userAttrs;
                switch (startAttr & PA_USERMASK) {
                case PA_BOOT:
                    userAttrs = "Boot";
                    break;
                case PA_USER:
                    userAttrs = "User";
                    break;
                case PA_KERNEL:
                    userAttrs = "Kernel";
                    break;
                default:
                    userAttrs = "undefined";
                }
                printf("%p...%p -> %08x %s %s\r\n", start, end - 1,
                    TRUNC(startEntry), attribs, userAttrs);
                start = end;
            } else {
                start += PAGESIZE;
            }
        } while (start != nullptr);
        printf("===================================================\r\n");
    }
    /**
     * Maps a memory kernel area. The memory is mapped 1:1 for kernel access
     * and is mapped into the virtual memory area defined by @ref CODE. The
     * 1:1 mapping is required, as the code is executed here before the
     * execution is transfered to the virtual address, and marking the memory
     * protects it from being allocated by other threads.
     *
     * The virtually mapped memory is mapped as globally available. It will be
     * mapped by all threads to the same memory area to be accessible from all
     * threads. Marking it as globally available leads into increased access
     * speed, as the corresponding mapping entries don't need to be updated
     * on task switches.
     */
    void mapKernel(
        const void* address,        ///< The start address of the memory area,
                                    ///< which will be mapped.
        const void* nextSegment,    ///< The first byte of the following
                                    ///< memory area, i.e. the first byte,
                                    ///< which will not be mapped.
        attr_e attributes
    ) {
        assert(address < nextSegment,
            "PagingDirectory::mapKernel(): negative size of memory area.");
        const uint32_t delta = &CODE - &PHYS;
        const char* virt = (const char*)address;
        const char* phys = virt - delta;
        const size_t size = (const char*)nextSegment - virt;
        map(phys, phys, size,
            (attr_e)((PA_KERNEL | PA_PRESENT | attributes) & ~PA_RING0));
        map(phys, virt, size,
            (attr_e)(PA_KERNEL | PA_GLOBAL | PA_PRESENT | attributes));
    }
public:
    /**
     * Returns the physical address to which a virtual address is mapped.
     * Asserts, if the virtual address is not mapped.
     *
     * @return The requested physical address.
     */
    void* getPhysicalAddress(
        const void* virtAddr    ///< The virtual address, for which the paging
                                ///< directory entry is saught.
    ) {
        return (void*)(TRUNC(*getTableEntry(virtAddr)) + ATTR(virtAddr));
    }
    /**
     * Initializes the system memory. The information provided by the multiboot
     * loader is taken into account. This applies especially to the maximum
     * boundaries of the lower and upper memory, to memory holes and to special
     * memory areas, such as loaded modules, BIOS memory, video memory and the
     * like.
     */
    static void init(multiboot_info_t* info);
    /**
     * Loads this into the memory management unit.
     */
    void load() {
        asm(
            "movl   %%eax, %%cr3" : :
            "a"(getPhysicalAddress(this)));
    }
};

/**
 * The paging directory for the kernel.
 */
extern PagingDirectory kernelPagingDirectory;

void PagingDirectory::init(multiboot_info_t* info) {
    uint32_t delta = &CODE - &PHYS;
    PagingDirectory* dir = (PagingDirectory*)((char*)&kernelPagingDirectory -
        delta);
    // adjust the virtual adresses of the paging directory to physical
    // addresses
    for (int i = 0; i < 1024; i++) {
        if (!ISFREE(dir->data[i])) {
            dir->data[i] -= delta;
        }
    }
    // map the kernel
    dir->mapKernel(&CODE, &READWRITE, PA_RING0);
    dir->mapKernel(&READWRITE, &READONLY, PA_WRITABLE);
    dir->mapKernel(&READONLY, &END, (attr_e)0);
    // enable paging
    dir->load();
    asm(
        "addl    %%eax, %%esp;"
        "movl    %%cr0, %%eax;"
        "orl     $0x80010000, %%eax;"
        "movl    %%eax, %%cr0" : :
        "a"(delta));
    if (info != nullptr) {
    }
}

}