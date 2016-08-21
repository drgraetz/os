#include "kernel.hpp"

/**
 * @file
 *
 * Implements the kernel's memory functioniality.
 *
 * @author Dr. Florian M. Grätz
 */

//#define IS_LOWMEM(x)		((x) < ((uint32_t)&CODE / AddressSpace::MEMPAGESIZE))
//
///**
// * Number of free pages of memory.
// */
//static uint32_t freePagesCount;
//
///**
// * A descriptor for a set of free memory pages. A descriptor fits exactliy in
// * a single memory page.
// */
//typedef struct freeMemoryDesc_s {
//    /**
//     * A set of indices to free memory pages.
//     */
//    uint32_t freeIndices[sizeof(AddressSpace::MemPage) / sizeof(uint32_t) - 1];
//    /**
//     * Pointer to the next structure in the single linked list, or INVALID_PTR.
//     */
//    struct freeMemoryDesc_s* next;
//} freeMemoryDesc_t;
//
///**
// * Pointer to a descriptor holding information on the free system memory.
// */
//freeMemoryDesc_t* freeMemory = (freeMemoryDesc_t*)INVALID_PTR;
///**
// * Number of unused entries in the currently used memory descriptor.
// */
//int freeEntriesInCurrentDescriptor = 0;
//
//bool swapLowMem(int& idx) {
//	for (freeMemoryDesc_t* act = freeMemory; !IS_LOWMEM(idx); act = act->next) {
//		if (act == INVALID_PTR) {
//			return false;
//		}
//		for (size_t i = 0; i < ARRAYSIZE(act->freeIndices); i++) {
//			uint32_t temp = act->freeIndices[i];
//			if (IS_LOWMEM(temp)) {
//				act->freeIndices[i] = idx;
//				idx = temp;
//			}
//		}
//	}
//	return true;
//}
//
//void MemoryManager::markAsFree(int idx) {
//	if (freeEntriesInCurrentDescriptor > 0) {
//        freeMemory->freeIndices[--freeEntriesInCurrentDescriptor] = idx;
//        freePagesCount++;
//        printf("%u: %u\r\n", freeEntriesInCurrentDescriptor, idx);
//        return;
//	}
//	freeMemoryDesc_t* phys;
//	if (!swapLowMem(idx)) {
//		return;
//	}
//	phys = (freeMemoryDesc_t*)(idx * MEMPAGE_SIZE);
//	printf("TABLE: %p\r\n", phys);
//	AddressSpace::kernel.map(phys, phys, MEMPAGE_SIZE, true, false);
//	AddressSpace::kernel.load();
//	phys->next = freeMemory;
//	freeMemory = phys;
//	freeEntriesInCurrentDescriptor = ARRAYSIZE(phys->freeIndices);
//}
//
//int MemoryManager::allocate(bool allowHighMemory) {
//	int result;
//    if (freePagesCount == 0) {
//        return -1;
//    }
//    if (freeEntriesInCurrentDescriptor == ARRAYSIZE(freeMemory->freeIndices)) {
//    	result = (uint32_t)freeMemory / MEMPAGE_SIZE;
//    	freeMemory = freeMemory->next;
//    	freeEntriesInCurrentDescriptor = 0;
//    } else {
//    	result = freeMemory->freeIndices[freeEntriesInCurrentDescriptor++];
//    	freePagesCount--;
//    }
//    if (!allowHighMemory) {
//    	if (!swapLowMem(result)) {
//    		return -1;
//    	}
//    }
//    return result;
//}
//
//uint32_t MemoryManager::getFreePagesCount() {
//    return freePagesCount;
//}

void* memset(void* ptr, int value, size_t byteCount) {
    char* c = (char*)ptr;
    for (; byteCount > 0; byteCount--) {
        *c++ = value;
    }
    return ptr;
}

//void* memcpy(void* dest, const void* source, size_t byteCount) {
//    char* c = (char*)dest;
//    for (; byteCount > 0; byteCount--) {
//        *c++ = *(const char*)source;
//        source = (char*)source + 1;
//    }
//    return dest;
//}
//
//MemoryStream::MemoryStream(void* buffer, size_t size, size_t pos) {
//    start = (char*)buffer;
//    end = (char*)buffer + size;
//    if (pos > size) {
//        pos = size;
//    }
//    current = (char*)buffer + pos;
//}
//
//off_t MemoryStream::seek(off_t pos) {
//    if (pos < 0 || pos > end - start) {
//        errno = EINVAL;
//        return -1;
//    }
//    current = start + pos;
//    errno = ESUCCESS;
//    return pos;
//}
//
//ssize_t MemoryStream::write(const void* buf, size_t size) {
//    if (buf == nullptr || size > SSIZE_MAX) {
//        errno = EINVAL;
//        return -1;
//    }
//    if (current + size > end) {
//        size = end - current;
//    }
//    memcpy(current, buf, size);
//    current += size;
//    return size;
//}
//
//ssize_t MemoryStream::read(void* buf, size_t size) {
//    if (buf == nullptr || size > SSIZE_MAX) {
//        errno = EINVAL;
//        return -1;
//    }
//    if (current + size > end) {
//        size = end - current;
//    }
//    memcpy(buf, current, size);
//    current += size;
//    return size;
//}
