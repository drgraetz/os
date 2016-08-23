#include "kernel.hpp"

AddressSpace AddressSpace::kernel;

void* AddressSpace::getPhysicalAddressImpl(const void* virtAddr) {
    return (void*)virtAddr;
}

#ifdef VERBOSE
void AddressSpace::dump() {
}
#endif
