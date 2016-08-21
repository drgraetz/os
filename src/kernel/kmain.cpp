#include "kernel.hpp"

/**
 * @file
 *
 * Holds the kernel's main function.
 */

template <class C> C* getKernelPtr(const C* value) {
    if (AddressSpace::isPagingEnabled()) {
        return (C*)value;
    } else {
        return AddressSpace::getPhysicalAddress(value);
    }
}

///**
// * The start of the list of module initializers. See @ref initModules() for
// * details. This symbols is defined by the linker script.
// */
//extern "C" void(*__CTOR_LIST__)();
///**
// * The end of the list of module initializers. See @ref initModules() for
// * details. This symbol is defined by the linker script.
// */
//extern "C" void(*__CTOR_END__)();
//
///**
// * Invokes all module initializers. This step is necessary, as no C runtime
// * library is available for the kernel. The linker scripts defines the start
// * and the end of the list of initializers. The list of initializers is an
// * array of function pointers. The initalizer functions are parameterless and
// * do not provide return values.
// */
//void initModules() {
//    for (void (**init)() = &__CTOR_LIST__; init < &__CTOR_END__; init++) {
//        if (*init != nullptr) {
//            (*init)();
//        }
//    }
//}

void kmain(struct boot_data_s& data) {
    uart::init();
//    initModules();
    printf("kmain(%p)\r\n", &data);
//    void* ebp;
//    void* esp;
//    void* eip;
//    asm(
//        "call   .loadIp;"
//        ".loadIp:;"
//        "popl   %%eax;" :
//        "=a"(eip) :);
//    asm(
//        "movl   %%ebp, %%eax;" :
//        "=a"(ebp));
//    asm(
//        "movl   %%esp, %%eax;" :
//        "=a"(esp));
//    printf("ebp=%p esp=%p eip=%p\r\n", ebp, esp, eip);
//    printf("%uk of free memory\r\n", MemoryManager::getFreePagesCount() * 4);
    halt();
}
