#include "kernel.hpp"

void AddressSpace::map(const void* virt, const void* phys, size_t size,
    bool write, bool user) {
    printf("AddressSpace::map(%p, %p, %u, %u, %u) @ %p\r\n", virt, phys, size, write, user, this);
    auto addressBitsPerLevel = getAddressBitsPerLevel();
    while (size > 0) {
        SafeArray<PageTableEntry> table = getPagingDirectory();
        PageTableEntry* entry = invalidPtr<PageTableEntry>();
        uintptr_t addr = (intptr_t)virt;
        size_t remainingBits = sizeof(intptr_t) * 8;
        size_t level = 0;
        auto bits = getAddressBitsPerLevel();
        while (*bits) {
            remainingBits -= *bits;
            intptr_t index = (addr >> remainingBits) & ~(-1 << *bits);
            entry = &table[index];
            level++;
            bits++;
            if (*bits) {
                if (entry->isEmpty()) {
                    // TODO: allocate memory for new paging table
                    assert(false);
                    halt();
                }
                table = SafeArray<PageTableEntry>(
                    (PageTableEntry*)entry->getPhysicalAddress(), 1 << *bits);
            }
        }
        assert(valid(entry));
        assert(entry->isEmpty());
        entry->set(phys, write, user, inKernel(virt), level);
        size_t pageSize = 1 << remainingBits;
        assert(size >= pageSize);
        size -= pageSize;
        virt = (void*)((uintptr_t)virt + pageSize);
        phys = (void*)((uintptr_t)phys + pageSize);
    }
}

void AddressSpace::dump() {
    assert(false);
}

const int* AddressSpace::getAddressBitsPerLevel() {
    if (isPagingEnabled()) {
        return ADDRESSBITSPERLEVEL;
    } else {
        return getPhysicalAddress(ADDRESSBITSPERLEVEL);
    }
}

SafeArray<PageTableEntry> AddressSpace::getPagingDirectory() {
    size_t entryCount = 1 << getAddressBitsPerLevel()[0];
    auto firstEntry = (PageTableEntry*)this;
    return SafeArray<PageTableEntry>(firstEntry, entryCount);
}

void AddressSpace::adjustTableAddresses() {
    for (PageTableEntry& entry : getPagingDirectory()) {
        if (!entry.isEmpty()) {
            const void* virt = entry.getPhysicalAddress();
            const void* phys = getPhysicalAddress(virt);
            entry.setPhysicalAddress(phys);
        }
    }
}

void AddressSpace::mapKernel(const void* virtAddr) {
    size_t readOnlySize = &KERNEL_READ_WRITE - &KERNEL_CODE;
    size_t readWriteSize = &KERNEL_END - &KERNEL_READ_WRITE;
    map(virtAddr, &PHYSICAL_ADDR, readOnlySize, false, false);
    map((const char*)virtAddr + readOnlySize,
        &PHYSICAL_ADDR + readOnlySize,
        readWriteSize,
        true,
        false);
}

void AddressSpace::init() {
    AddressSpace& kernel = *getPhysicalAddress(&AddressSpace::kernel);
    kernel.adjustTableAddresses();
    kernel.mapKernel(&PHYSICAL_ADDR);
    kernel.mapKernel(&KERNEL_CODE);
    kernel.dump();
    kernel.load();
    AddressSpace::enablePaging();
    AddressSpace::adjustStack();
}

void* AddressSpace::getPhysicalAddressImpl(const void* virtAddr) {
    if (virtAddr >= &KERNEL_CODE) {
        size_t delta = &KERNEL_CODE - &PHYSICAL_ADDR;
        return (void*)((const char*)virtAddr - delta);
    }
    if (!isPagingEnabled()) {
        return (void*)virtAddr;
    }
    return invalidPtr<void>();
}
