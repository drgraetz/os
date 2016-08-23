#include "kernel.hpp"

AddressSpace AddressSpace::kernel;

void* AddressSpace::getPhysicalAddressImpl(const void* virtAddr) {
    return (void*)virtAddr;
}

bool AddressSpace::isPagingEnabled() {
    return false;
}

#ifdef VERBOSE
void AddressSpace::dump() {
}
#endif
