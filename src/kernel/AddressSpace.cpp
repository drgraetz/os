#include "kernel.hpp"

void AddressSpace::mapKernel(const void* virtAddr) {
    size_t readOnlySize = &KERNEL_READ_WRITE - &KERNEL_CODE;
    size_t readWriteSize = ((&KERNEL_END - &KERNEL_READ_WRITE) +
        AddressSpace::getPageSize() - 1) & ~(AddressSpace::getPageSize() - 1);
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
    kernel.load();
    AddressSpace::enablePaging();
}


void* AddressSpace::getPhysicalAddressImpl(const void* virtAddr) {
    if (virtAddr >= &KERNEL_CODE) {
        size_t delta = &KERNEL_CODE - &PHYSICAL_ADDR;
        return (void*)((const char*)virtAddr - delta);
    }
    // return invalidPtr<void>();
    return (void*)virtAddr;
}
