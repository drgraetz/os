#include "kernel.hpp"

/**
 * @file
 *
 * Implements the kernel's memory functioniality.
 */

/**
 * Number of free pages of memory.
 */
static uint32_t freePagesCount;

/**
 * A descriptor for a set of free memory pages. A descriptor fits exactliy in
 * a single memory page.
 */
typedef struct freeMemoryDesc_s {
    /**
     * Returns the number of valid entries in the top most memory descriptor.
     */
    static uint32_t numberOfValidEntries() {
        return MemoryManager::getFreePagesCount() % ARRAYSIZE(freeIndices);
    }
    /**
     * A set of indices to free memory pages.
     */
    uint32_t freeIndices[MEMPAGE_SIZE / sizeof(uint32_t) - 1];
    /**
     * Pointer to the next structure in the single linked list, or INVALID_PTR.
     */
    struct freeMemoryDesc_s* next;
} freeMemoryDesc_t;

/**
 * Pointer to a descriptor holding information on the free system memory.
 */
freeMemoryDesc_t* freeMemory = (freeMemoryDesc_t*)INVALID_PTR;

void MemoryManager::markAsFree(const uint32_t idx) {
    uint32_t entryIndex = freeMemoryDesc_t::numberOfValidEntries();
    if (freeMemory == INVALID_PTR ||
        entryIndex == ARRAYSIZE(freeMemoryDesc_t::freeIndices) - 1) {
        freeMemoryDesc_t* phys = (freeMemoryDesc_t*)(idx * MEMPAGE_SIZE);
        AddressSpace::kernel.map(phys, phys, MEMPAGE_SIZE, true, false);
        AddressSpace::kernel.load();
        phys->next = freeMemory;
        freeMemory = phys;
        printf("Memory table created: %u\r\n", freeMemoryDesc_t::numberOfValidEntries());
    } else {
        freeMemory->freeIndices[entryIndex] = idx;
        printf("%04u: %04u\r\n", entryIndex, idx);
        freePagesCount++;
    }
}

uint32_t MemoryManager::allocate() {
    if (freePagesCount == 0) {
        return -1;
    }
    uint32_t idx = freeMemoryDesc_t::numberOfValidEntries();
    uint32_t result = freeMemory->freeIndices[idx];
    freePagesCount--;
    return result;
}

uint32_t MemoryManager::getFreePagesCount() {
    return freePagesCount;
}

void* memset(void* ptr, int value, size_t byteCount) {
    char* c = (char*)ptr;
    for (; byteCount > 0; byteCount--) {
        *c++ = value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* source, size_t byteCount) {
    char* c = (char*)dest;
    for (; byteCount > 0; byteCount--) {
        *c++ = *(const char*)source;
        source = (char*)source + 1;
    }
    return dest;
}

MemoryStream::MemoryStream(void* buffer, size_t size, size_t pos) {
    start = (char*)buffer;
    end = (char*)buffer + size;
    if (pos > size) {
        pos = size;
    }
    current = (char*)buffer + pos;
}

off_t MemoryStream::seek(off_t pos) {
    if (pos < 0 || pos > end - start) {
        errno = EINVAL;
        return -1;
    }
    current = start + pos;
    errno = ESUCCESS;
    return pos;
}

ssize_t MemoryStream::write(const void* buf, size_t size) {
    if (buf == nullptr || size > SSIZE_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (current + size > end) {
        size = end - current;
    }
    memcpy(current, buf, size);
    current += size;
    return size;
}

ssize_t MemoryStream::read(void* buf, size_t size) {
    if (buf == nullptr || size > SSIZE_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (current + size > end) {
        size = end - current;
    }
    memcpy(buf, current, size);
    current += size;
    return size;
}
