#include "kernel.hpp"

extern "C" void handleException(
    uint32_t eip,
    uint32_t edi, uint32_t esi, uint32_t ebp, uint32_t esp,
    uint32_t ebx, uint32_t edx, uint32_t ecx, uint32_t eax,
    uint16_t gs, uint16_t fs, uint16_t es, uint16_t ds,
    uint32_t number, uint32_t errorCode) {
    printf("EXCEPTION %02X (%08x) eip=%08x:\r\n", number, errorCode, eip);
    printf("ds=%04x es=%04x fs=%04x gs=%04x\r\n", ds, es, fs, gs);
    printf("eax=%08x ebx=%08x ecx=%08x edx=%08x\r\n", eax, ebx, ecx, edx);
    printf("esp=%08x ebp=%08x esi=%08x edi=%08x\r\n", esp, ebp, esi, edi);
    halt();
}

extern "C" void __cxa_pure_virtual() {
    printf("Pure virtual function called.\r\n");
    halt();
}
