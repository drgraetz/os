#include "kernel.hpp"

/**
 * @file
 * The implementation of an address space for arm processors.
 *
 * @author Dr. Florian Manfred Grätz
 *
 * A good description of memory management on ARM processors can be obtained
 * from the ARM documentation, chapter Memory Management Unit
 * <http://infocenter.arm.com/help/topic/com.arm.doc.ddi0333h/Babbhigi.html>
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
} memoryAttributes_e;

/**
 * An entry in the paging directory.
 */
class PageDirEntry {
private:
    uint32_t data;
public:
    /**
     * Checks, whether this is an invalid descriptor.
     */
    inline bool isEmpty() {
        return (data & PA_TYPE_MASK) == PA_INVALID;
    }
    /**
     * Returns the physical address, this points to.
     */
    inline void* getPhysicalAddress() {
        switch (data & PA_TYPE_MASK) {
        case PA_COARSE:
            return (void*)(data & 0xFFFFFC00);
        case PA_SECTION:
            return (void*)(data & 0xFFF00000);
        }
        return invalidPtr<void>();
    }
    /**
     * Returns the page attributes of this.
     */
    inline memoryAttributes_e getAttributes() {
        switch (data & PA_TYPE_MASK) {
        case PA_COARSE:
            return (memoryAttributes_e)(data & 0x000003FF);
        case PA_SECTION:
            return (memoryAttributes_e)(data & 0x000FFFFF);
        }
        return PA_INVALID;
    }
    /**
     * Sets the value of this.
     */
    inline void set(const void* physAddr, memoryAttributes_e attrs) {
        data = (uint32_t)physAddr | attrs;
    }
    /**
     * Adjusts the address of this from the virtual kernel memory area to
     * its physical address.
     */
    inline void adjustAddress() {
        if (!isEmpty()) {
            void* virt = getPhysicalAddress();
            void* phys = AddressSpace::getPhysicalAddress(virt);
            set(phys, getAttributes());
        }
    }
};

/**
 * A paging directory.
 */
class PageDirectory {
private:
    PageDirEntry content[4096];
public:
    /**
     * Returns the entry at a given index.
     */
    PageDirEntry& operator[](size_t i) {
        return content[i];
    }
};

void AddressSpace::adjustTableAddresses() {
    PageDirectory& dir = *(PageDirectory*)this;
    for (size_t i = 0; sizeof(PageDirectory) / sizeof(PageDirEntry); i++) {
        dir[i].adjustAddress();
    }
}

void AddressSpace::load() {
}

void AddressSpace::map(const void* virtAddr, const void* physAddr, size_t size,
    bool writable, bool userAccess) {
    printf("map(%p, %p, %u, %u, %u)\r\n", virtAddr, physAddr, size, writable, userAccess);
}

void AddressSpace::enablePaging() {
}

void AddressSpace::adjustStack() {
}

#ifdef VERBOSE
void AddressSpace::dump() {
    printf("===========================================\r\n");
    printf("Paging Directory @ %p\r\n", this);
    printf("===========================================\r\n");
}
#endif
