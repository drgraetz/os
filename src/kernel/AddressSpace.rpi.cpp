#include "kernel.hpp"

AddressSpace AddressSpace::kernel;

void AddressSpace::adjustTableAddresses() {
    /*PageDirectory& dir = *(PageDirectory*)this;
    for (size_t i = 0; sizeof(PageDirectory) / sizeof(PageTableEntry); i++) {
        dir[i].adjustAddress();
    }*/
    assert(false);
}

void AddressSpace::load() {
    assert(false);
}

void AddressSpace::map(
    const void* virtAddr,
    const void* physAddr,
    size_t size,
    bool writable,
    bool userAccess) {
    assert(false);
}

size_t AddressSpace::getPageSize() {
    return 4096;
}

void AddressSpace::enablePaging() {
}

#ifdef VERBOSE
void AddressSpace::dump() {
    printf("===========================================\r\n");
    printf("Paging Directory @ %p\r\n", this);
    printf("===========================================\r\n");
}
#endif
