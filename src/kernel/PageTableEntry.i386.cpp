#include "kernel.hpp"

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
    PA_OS0 = 0x000,     ///< Page is of OS specific type 0.
    PA_OS1 = 0x200,     ///< Page is of OS specific type 1.
    PA_OS2 = 0x400,     ///< Page is of OS specific type 2.
    PA_OS3 = 0x600,     ///< Page is of OS specific type 3.
    PA_OS4 = 0x800,     ///< Page is of OS specific type 4.
    PA_OS5 = 0xA00,     ///< Page is of OS specific type 5.
    PA_OS6 = 0xC00,     ///< Page is of OS specific type 6.
    PA_OS7 = 0xE00,     ///< Page is of OS specific type 7.
    PA_OS_MASK = PA_OS7 ///< Masks the specific bits.
} pageAttributes_e;

bool PageTableEntry::isEmpty() {
    return (data & PA_PRESENT) == 0;
}

void* PageTableEntry::getPhysicalAddress() {
    return (void*)(data & 0xFFFFF000);
}

void PageTableEntry::setPhysicalAddress(const void* addr) {
    uintptr_t value = (uintptr_t)addr;
    assert(!isEmpty());
    assert((value & 0xFFFFF000) == value);
    data = (data & 0x00000FFF) | value;
}

void PageTableEntry::set(const void* addr, bool write, bool user, bool global,
    size_t level) {
    uintptr_t value = (uintptr_t)value | PA_PRESENT;
    assert(isEmpty());
    assert((value & 0xFFFFF000) == value);
    if (global) {
        value |= PA_GLOBAL;
    }
    if (user) {
        value |= PA_RING0;
    }
    if (write) {
        value |= PA_WRITABLE;
    }
    data = value;
}
