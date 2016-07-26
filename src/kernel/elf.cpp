#include "kernel.hpp"
#include <elfdefs.h>

/**
 * The header of an ELF file.
 */
typedef struct __attribute__((packed)) {
    uint32_t    magic;
    enum : uint8_t {
        ELF_CLASS_NONE = 0,
        ELF_CLASS_32 = 1,
        ELF_CLASS_64 = 2
    } elfClass;
    enum : uint8_t {
        ELF_DATA2LSB = 1,
        ELF_DATA2MSB = 2
    } data;
    uint8_t     headerVersion;
    uint8_t     padding[9];
    enum : uint16_t {
        ET_NONE = 0,
        ET_REL = 1,
        ET_EXEC = 2,
        ET_DYN = 3,
        ET_CORE = 4,
    } type;
    enum : uint16_t {
        EM_NONE = 0,
        EM_M32 = 1,
        EM_SPARC = 2,
        EM_386 = 3,
        EM_68K = 4,
        EM_88K = 5,
        EM_860 = 7,
        EM_MIPS = 8
    } elfMachine;
    uint32_t    version;
    uint32_t    entry;
    uint32_t    programHeader;
    uint32_t    symbolHeader;
    uint32_t    flags;
    uint16_t    headerSize;
    uint16_t    programHeaderSize;
    uint16_t    programHeaderCount;
    uint16_t    symbolHeaderSize;
    uint16_t    symbolHeaderCount;
    uint16_t    stringTableIndex;
} ElfHeader;

/**
 * A program header within an ELF file.
 */
typedef struct __attribute__((packed)) {
    enum : uint32_t {
        PT_NULL = 0,
        PT_LOAD = 1,
        PT_DYNAMIC = 2,
        PT_INTERP = 3,
        PT_NOTE = 4,
        PT_SHLIB = 5,
        PT_PHDR = 6,
        PT_GNU_STACK = 0x6474E551
    } type;
    uint32_t offset;
    uint32_t virtAddr;
    uint32_t physAddr;
    uint32_t fileSize;
    uint32_t memorySize;
    uint32_t flags;
    uint32_t alignment;
} ProgramHeader;

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
uint32_t ntohl(uint32_t value) {
    return value;
}
#define ELFDATA         ELF_DATA2MSB
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
uint32_t ntohl(uint32_t value) {
    return
        ((value >> 24) & 0x000000FF) |
        ((value >>  8) & 0x0000FF00) |
        ((value <<  8) & 0x00FF0000) |
        ((value << 24) & 0xFF000000);
}
#define ELFDATA         ELF_DATA2LSB
#else
#error "Unsuppported byte order."
#endif

errno_e loadElf(Stream& stream) {
    ElfHeader hdr;
    printf("loading elf, sizeof(hdr) = %u\r\n", sizeof(hdr));
    if (stream.read(&hdr, sizeof(hdr)) != sizeof(hdr) ||
        ntohl(hdr.magic) != 0x7F454C46 || hdr.elfClass != ELFCLASS ||
        hdr.data != ElfHeader::ELFDATA) {
        return ENOEXEC;
    }
    if (stream.seek(hdr.programHeader) != hdr.programHeader) {
        return ENOEXEC;
    }
    for (size_t i = 0; i < hdr.programHeaderCount; i++) {
        ProgramHeader ph;
        if (stream.read(&ph, sizeof(ph)) != sizeof(ph)) {
            return ENOEXEC;
        }
        printf("%u %u @%p(%p) *%u *%u %08x %u\r\n",
            ph.type, ph.offset, ph.virtAddr, ph.physAddr, ph.fileSize,
            ph.memorySize, ph.flags, ph.alignment);
    }
    AddressSpace* as = AddressSpace::create();
    return ESUCCESS;
}
