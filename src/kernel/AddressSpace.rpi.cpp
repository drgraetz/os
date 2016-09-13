#include "kernel.hpp"

/**
 * @file
 *
 * The armv7-specific implementation of the @ref AddressSpace class.
 *
 * @author Dr. Florian M. Grätz
 */

void AddressSpace::load() {
    assert(false);
}

void AddressSpace::enablePaging() {
    assert(false);
}

void AddressSpace::adjustStack() {
    assert(false);
}

bool AddressSpace::isPagingEnabled() {
    return false;
}

const int AddressSpace::ADDRESSBITSPERLEVEL[] = {12, 8, 0};

#ifdef X

class Entry {
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
     * Sets the value of this.
     */
    inline void set(const void* physAddr, memoryAttributes_e attrs) {
        assert(((attrs & PA_TYPE_MASK) == PA_COARSE &&
                ((uint32_t)physAddr & 0x3FF) == 0) ||
            ((attrs & PA_TYPE_MASK) == PA_SECTION &&
                ((uint32_t)physAddr & 0xFFFFF) == 0));
        data = (uint32_t)physAddr | attrs;
    }
    /**
     * Sets the physical address of this.
     */
    inline void setPhysicalAddress(const void* physAddr) {
        set(physAddr, getAttributes());
    }
};

class PageDir : public Table<4096, Entry> {
};

void AddressSpace::dump() {
    assert(false);
}

void AddressSpace::load() {
    assert(false);
}

void AddressSpace::enablePaging() {
    assert(false);
}

void AddressSpace::adjustTableAddresses() {
    ((PageDir*)this)->adjustAddresses();
}

void AddressSpace::adjustStack() {
    assert(false);
}

void AddressSpace::map(const void* virt, const void* phys, size_t size,
    bool write, bool user) {
   ((PageDir*)this)->map(virt, phys, size, write, user);
}

/**
 * The size of a memory page in bytes.
 */
#define PAGESIZE        4096
/**
 * The size of a large memory page in bytes.
 */
#define LARGEPAGESIZE   (1024 * 1024)

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
    ((PageDir*)this)->adjustAddresses();
}

void AddressSpace::map(const void* virtAddr, const void* physAddr, size_t size,
    bool writable, bool userAccess) {
    assert(false);
/*    printf("map(%p, %p, %u, %u, %u)\r\n", virtAddr, physAddr, size, writable, userAccess);
    assert(AddressSpaceImpl::aligned(size) &&
        AddressSpaceImpl::aligned(virtAddr) &&
        AddressSpaceImpl::aligned(physAddr));
    while (size != 0) {

    }*/
}

#ifdef VERBOSE
void AddressSpace::dump() {
    printf("===========================================\r\n");
    printf("Paging Directory @ %p\r\n", this);
    printf("===========================================\r\n");
}
#endif
#endif
