#include "kernel.hpp"

AddressSpace AddressSpace::kernel;

void* AddressSpace::getPhysicalAddressImpl(const void* virtAddr) {
    return (void*)virtAddr;
}

#ifdef VERBOSE
void AddressSpace::dump() {
    printf("===========================================\r\n");
    printf("Paging Directory @ %p\r\n", this);
    printf("===========================================\r\n");
}
#endif
