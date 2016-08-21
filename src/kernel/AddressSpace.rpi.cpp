#include "kernel.hpp"

void* AddressSpace::getPhysicalAddressImpl(const void* virtAddr) {
    return (void*)virtAddr;
}

bool AddressSpace::isPagingEnabled() {
    return false;
}
