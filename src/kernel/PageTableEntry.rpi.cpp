#include "kernel.hpp"

/**
 * @file
 *
 * The armv7-specific implementation of the @ref PageTableEntry class.
 *
 * @author Dr. Florian M. Grätz
 */

/**
 * The attribute definition for a memory page.
 */
typedef enum {
    PA_INVALID = 0,         ///< Invalid page table entry.
    PA_COARSE = 1,          ///< Coarse page table with 256 entries.
    PA_SECTION = 2,         ///< Section page (no table, blank memory)
    PA_FINE = 3,            ///< Fine page table with 1024 entries, unavailable
                            ///< on machines compliant with VMSAv6 or newer.
    PA_TYPE_MASK = 3,       ///< Mask for masking the page type (i.e.
                            ///< PA_INVALID, PA_COARSE, PA_SECTION, or PA_FINE)
    PA_BUFFERED = 4,        ///< Section is buffered.
    PA_CACHED = 8,          ///< Section is cached.
    PA_DOMAIN_MASK = 0x1E0, ///< Masks the domain number.
} pageAttributes_e;

bool PageTableEntry::isEmpty() {
    return (data & PA_TYPE_MASK) == PA_INVALID;
}

void* PageTableEntry::getPhysicalAddress() {
    switch (data & PA_TYPE_MASK) {
    case PA_COARSE:
        return (void*)(data & 0xFFFFFC00);
    default:
        return invalidPtr<void>();
    }
}

void PageTableEntry::setPhysicalAddress(const void* addr) {
    uintptr_t value = (uintptr_t)addr;
    switch (data & PA_TYPE_MASK) {
    case PA_COARSE:
        assert((value & 0xFFFFFC00) == value);
        data = (data & 0x000003FF) | value;
        break;
    default:
        assert(false);
    }
}

void PageTableEntry::set(const void* addr, bool write, bool user, bool global,
    size_t level) {
    uintptr_t value = (uintptr_t)addr;
    assert(false);
}
