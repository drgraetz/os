#include <multiboot/multiboot.h>
#include "kernel.hpp"

/**
 * @file
 *
 * The i386-specific implementation of the address mangement.
 *
 * @author Dr. Florian Manfred Grätz
 */

/**
 * The first byte of the kernel in the virtual memory. The address is
 * defined in buildinfo.xml.
 */
extern const char kernelAddr;
/**
 * The first byte of the kernel in the physical memory. The address is
 * defined in buildinfo.xml.
 */
extern const char bootAddr;

///**
// * Truncates an address to the start of the corresponding memory page. The
// * result is uint32_t and therefore must be casted into a pointer type in most
// * cases.
// */
//#define TRUNC(x)        ((uint32_t)(x) & ~(MEMPAGE_SIZE - 1))
///**
// * Yields to the attributes of a paging dir/table entry or the offset within
// * a memory page.
// */
//#define ATTR(entry)     (((uint32_t)entry) & (MEMPAGE_SIZE - 1))
///**
// * True, if a given directory/table entry is unused.
// */
//#define ISUNUSED(x)     ((x) == 0)
///**
// * Computes a physical address for a virtual kernel address.
// */
//#define PHYSADDR(x)     ((void*)((char*)x - (uint32_t)(&CODE - &PHYS)))
///**
// * The index in a paging directory, that corresponds to an address.
// */
//#define DIRINDEX(x)     ((uint32_t)(x) >> 22)
///**
// * The index in a paging table, that corresponds to an address.
// */
//#define TABLEINDEX(x)   (((uint32_t)(x) >> 12) & 1023)
//
///**
// * The start of the kernel code in the physical address range.
// */
//extern const char PHYS;
///**
// * The start of the kernel code in the virtual address range.
// */
//extern const char CODE;
///**
// * The start of the data area in the virtual address range.
// */
//extern const char READWRITE;
///**
// * The start of the constant data area in the virtual address range.
// */
//extern const char READONLY;
///**
// * The end of the kernel in the virtual address range.
// */
//extern const char END;
//
//namespace i386 {
//
///**
// * Turns on paging and write protection in the virtual memory management unit.
// */
//static inline void enablePaging() {
//    asm(
//        "movl   %%cr0, %%eax;"
//        "orl    $0x80010000, %%eax;"
//        "movl   %%eax, %%cr0;" : :);
//}
//
///**
// * A paging table or paging directory. The upper 20 bits of every entry point
// * to an address, the lower 12 bits define a memory page's attribute.
// * @ref pageAttributes_e for a detailed description of possible page
// * attributes.
// */
//typedef struct {
//    uint32_t& operator[](size_t i) { return data[i]; }
//    uint32_t data[1024];
//} __attribute__((aligned(4096))) pageTable_t;
//
///**
// * The attribute definition for a memory page.
// */
//typedef enum {
//    PA_PRESENT = 1,     ///< Physically present in memory.
//    PA_WRITABLE = 2,    ///< Write access allowed.
//    PA_RING0 = 4,       ///< Accessible by user code.
//    PA_WRITETHRU = 8,   ///< Write through caching enabled.
//    PA_NOCACHE = 16,    ///< Caching disabled.
//    PA_ACCESSED = 32,   ///< Set by read operation on memory in page.
//    PA_DIRTY = 64,      ///< Set by write operation on memory in page.
//    PA_4MBYTE = 128,    ///< Directory entry discrables 4Mbyte page instead
//                        ///< of page table.
//    PA_GLOBAL = 256,    ///< Entry is used globally, i.e. it is not
//                        ///< updated, when a new table is loaded.
//} pageAttributes_e;
//
///**
// * The i386 specific implementation of an address space.
// */
//class AddressSpaceImpl : public AddressSpace {
//
////class TSS {
///*private:
//    struct {
//        TSS*                prev;   ///< When using hardware task switching,
//                                    ///< this value is set to the previous TSS
//                                    ///< by the processor.
//        struct {
//            void*           esp;    ///< The stack pointer.
//            uint32_t        ss;     ///< The stack segment.
//        } stack[3];                 ///< For the privelege levels 0...2 each
//                                    ///< stack is saved.
//        AddressSpaceImpl*   pageDir;///< The pagining directory used.
//        void*               eip;    ///< The instruction pointer.
//        uint32_t            eflags; ///< The processor flags.
//        uint32_t            eax;    ///< The eax register.
//        uint32_t            ecx;    ///< The ecx register.
//        uint32_t            edx;    ///< The edx register.
//        uint32_t            ebx;    ///< The ebx register.
//        void*               esp;    ///< The stack pointer.
//        void*               ebp;    ///< The base pointer.
//        void*               esi;    ///< The esi register.
//        void*               edi;    ///< The edi register.
//        uint32_t            es;     ///< The es register.
//        uint32_t            cs;     ///< The code segment.
//        uint32_t            ss;     ///< The stack segment.
//        uint32_t            ds;     ///< The data segment.
//        uint32_t            fs;     ///< The fs segment.
//        uint32_t            gs;     ///< The gs segment.
//        void*               ldt;    ///< The local descriptor table.
//        uint16_t            trap;   ///< When bit 0 is set, a debug trap will
//                                    ///< be triggered on each change by loading
//                                    ///< a new TSS.
//        uint16_t            iomap;  ///< Offset of the IO map. Each IO port
//                                    ///< is represented by a single bit. An
//                                    ///< I/O may only be accessed, if the
//                                    ///< corresponding bit is 0.
//    } __attribute__((packed)) data;
//public:
//    void load() {
//        asm(
//            "movw   %%ss, %%ax" : "=a"(data.stack[0].ss)
//        );
//        asm(
//            "movl   %%esp, %%eax" : "=a"(data.stack[0].esp)
//        );
//        printf("%08x:%p", data.stack[0], data.stack[0].esp);
//        printf("cpus[%u]", this - cpus);
//    }*/
//    /*
//     * For each CPU a TSS is required.
//     *//*
//    //static TSS cpus[4];
////};
//
////TSS TSS::cpus[4];
//
////}*/
//public:
//    /**
//     * The contents of this. The topmost 10 bits of every pointer select an
//     * entry in this. If the @ref PA_4MBYTE bit is set, the lower 22 bits
//     * select an entry in the 4 MByte page pointed to by the directory entry.
//     * Otherwise, the bits 12 ... 21 select an entry in the paging table
//     * pointed to by the entry in the directory. In the latter case, the lowest
//     * 12 bits define an offset in the 4 KByte page pointed to by the page
//     * table.
//     */
//    pageTable_t contents;
//    /**
//     * Maps a single page of memory. The virtual address has either to be
//     * unused or unmarked or already be paged to the same physical address
//     * with identical paging attributes.
//     *
//     * @return True on success. False on failure. In this case, errno will be
//     * set.
//     */
//    bool mapPage(
//        const void* virt,   ///< The virtual address, which is mapped. Must be
//                            ///< aligned to a page boundary.
//        const void* phys,   ///< The physical address, to which is mapped. Must
//                            ///< be aligned to a page boundary.
//        uint32_t attr       ///< The attributes, which will be applied to the
//                            ///< mapped memory page. See @ref pageAttributes_e
//                            ///< for details.
//    ) {
//        if (TRUNC(phys) != (uint32_t)phys || TRUNC(virt) != (uint32_t)(virt) ||
//            attr != ATTR(attr)) {
//            errno = EINVAL;
//            return false;
//        }
//        pageTable_t& dir = *(pageTable_t*)getPhysicalAddress(this);
//        uint32_t& dirEntry = dir[DIRINDEX(virt)];
//        pageTable_t* table;
//        if (ISUNUSED(dirEntry)) {
//            int idx = MemoryManager::allocate(false);
//            if (idx == -1) {
//            	errno = ENOMEM;
//            	return false;
//            }
//			table = (pageTable_t*)(idx * MEMPAGE_SIZE);
//			//dirEntry = (uint32_t)table | PA_WRITABLE | PA_PRESENT;
//			kernel.load();
//			printf("%p\r\n", table);
//			dump();
//			memset(table, 0, MEMPAGE_SIZE);
//			halt();
//        } else {
//            table = (pageTable_t*)TRUNC(dirEntry);
//        }
//		uint32_t& tableEntry = table->data[TABLEINDEX(virt)];
//		uint32_t value = (uint32_t)phys | attr;
//		if (ISUNUSED(tableEntry)) {
//			tableEntry = value;
//		} else if ((tableEntry & ~(PA_DIRTY | PA_ACCESSED)) != value) {
//			errno = EPERM;
//			return false;
//		}
//		errno = ESUCCESS;
//        return true;
//    }
//
//    #ifdef VERBOSE
//    /**
//     * Gets the value of an entry in the paging table for a virtual address.
//     *
//     * @return the requested value, or 0 if no paging table exists for the
//     * requested virtual address.
//     */
//    uint32_t getEntry(
//        const void* virt    ///< The virtual address, for which the entry is
//                            ///< requested.
//    ) {
//        uint32_t result;
//        const uint32_t dir = contents[DIRINDEX(virt)];
//        if (ISUNUSED(dir)) {
//            result = 0;
//        } else {
//            const pageTable_t* table = (pageTable_t*)TRUNC(dir);
//            result = table->data[TABLEINDEX(virt)];
//        }
//        return result;
//    }
//    #endif
//
//    /**
//     * Adjusts all addresses within in address space, which are mapped to the
//     * kernel data area, to the physical memory addresses.
//     *
//     * @return the physical address of the address space, which has been
//     * adjusted.
//     */
//    static AddressSpaceImpl* adjustAddresses(
//        AddressSpace* virt  ///< The virtual address of the address space,
//                            ///< which will be adjusted.
//    ) {
//        AddressSpaceImpl* phys = (AddressSpaceImpl*)PHYSADDR(virt);
//        for (int i = 0; i < 1024; i++) {
//            if (phys->contents[i] >= (uint32_t)&CODE) {
//                phys->contents[i] = (uint32_t)PHYSADDR(phys->contents[i]);
//            }
//        }
//        return phys;
//    }
//
//    /**
//     * Maps the kernel 1:1 and into its high memory address.
//     */
//    void mapKernel() {
//        const char* end = (const char*)TRUNC(&END + MEMPAGE_SIZE - 1);
//        map(PHYSADDR(&CODE), PHYSADDR(&CODE), end - &CODE, true, false);
//        map(&CODE, PHYSADDR(&CODE), &READWRITE - &CODE, false, true);
//        map(&READWRITE, PHYSADDR(&READWRITE), &READONLY - &READWRITE, true,
//            false);
//        map(&READONLY, PHYSADDR(&READONLY), end - &READONLY, false, false);
//    }
//
//    /**
//     * Adjusts the stack so that all eip and ebp values pushed to it will point
//     * to the coresponding virtual kernel addresses.
//     */
//    static void adjustStack() {
//        void** ptr;
//        asm(
//            "movl   %%ebp, %%eax" :
//            "=a"(ptr) :);
//        void** end = (void**)(TRUNC(ptr) + MEMPAGE_SIZE - 1);
//        uint32_t delta = &CODE - &PHYS;
//        while (ptr < end) {
//            void** next = (void**)*ptr;
//            void** ip = ptr + 1;
//            *ip = (void*)((uint32_t)*ip + delta);
//            *ptr = (void*)((uint32_t)next + delta);
//            ptr = next;
//        }
//        asm(
//            "addl   %%eax, %%esp;" : :
//            "a"(delta));
//    }
//
//    /**
//     * Unmaps all memory pages except for those, which are required by the
//     * kernel. These are:
//     * - the high mapped kernel memory area
//     * - the paging directory entries
//     */
//    void unmapAll() {
//        // loop over all tables except for the last one, as it describes the
//        // high mapped kernel
//        for (size_t i = 0; i < 1023; i++) {
//            if (ISUNUSED(contents[i])) {
//                continue;
//            }
//            pageTable_t& table = *(pageTable_t*)TRUNC(contents[i]);
//            for (size_t j = 0; j < 1024; j++) {
//                if (ISUNUSED(table[j])) {
//                    continue;
//                }
//                // check, wether the address is referenced by the kernel's
//                // paging directory
//                uint32_t addr = (i << 22) + (j << 12);
//                bool found = addr == TRUNC(this);
//                for (size_t k = 0; k < 1024 && !found; k++) {
//                    found = TRUNC(contents[k]) == addr;
//                }
//                if (!found) {
//                    table[j] = 0;
//                }
//            }
//        }
//    }
//
//    /**
//     * Maps a memory area provided by the bootloader into the kernel's address
//     * space. The mapped memory area will be extended to the closest page
//     * boundaries, so that at least the entire requested memory range is
//     * mapped.
//     *
//     * @return The virtual address to which the data is mapped. This value will
//     * have the same offset to the closest page boundary like the virtual
//     * address.
//     */
//    static const void* mapBootData(
//        const void*& virt,  ///< The next available virtual address. This
//                            ///< address must be aligned to a page boundary and
//                            ///< its value will be updated to the next page
//                            ///< after the newly mapped memory area.
//        const void* phys,   ///< The physical address to be mapped. This
//                            ///< address does not need to be aligned to a page
//                            ///< boundary.
//        size_t size         ///< The size of the memory to be mapped. This
//                            ///< value does not need to be a multiple of the
//                            ///< page size.
//    ) {
//        uint32_t offset = ATTR(phys);
//        size = TRUNC(size + offset + MEMPAGE_SIZE - 1);
//        kernel.map(virt, (void*)TRUNC(phys), size, false, false);
//        void* result = (void*)((char*)virt + offset);
//        virt = (const void*)((char*)virt + size);
//        return result;
//    }
//
//    /**
//     * Reports free memory blocks to the @ref MemoryManager. Every memory block
//     * in question will be checked, whether it is used by the kernel. In this
//     * case, it will not be reported as free memory.
//     */
//    static void reportFree(
//        const void* phys,   ///< The physical address of the memory block. Must
//                            ///< be aligned to a page boundary.
//        size_t size         ///< The size of the memory range. Must be a
//                            ///< multiple of the page size.
//    ) {
//        if (ATTR(size) != 0 || ATTR(phys) != 0) {
//            return;
//        }
//        if (((char*)phys + size < (char*)phys) && ((char*)phys + size != 0)) {
//            size = (char*)0 - (char*)phys;
//        }
//        uint32_t old = (uint32_t)phys;
//        for (; size > 0; size -= MEMPAGE_SIZE) {
//            bool used = false;
//            if (phys < &CODE) {
//                uint32_t dir = ((AddressSpaceImpl*)&kernel)->
//                    contents[DIRINDEX(phys)];
//                if (!ISUNUSED(dir)) {
//                    pageTable_t& table = *(pageTable_t*)TRUNC(dir);
//                    used = !ISUNUSED(table[TABLEINDEX(phys)]);
//                }
//            }
//            if (!used) {
//                MemoryManager::markAsFree((uint32_t)phys >> 12);
//            }
//            phys = (void*)((char*)phys + MEMPAGE_SIZE);
//        }
//    }
//
//    /**
//     * Evaluates the information provided by the multiboot infostructure. The
//     * data is mapped after the kernel's end.
//     */
//    static void evaluateMultibootData(const multiboot_info_t* info) {
//        const void* dest = (const char*)(TRUNC(&END + MEMPAGE_SIZE - 1));
//        info = (const multiboot_info_t*)mapBootData(dest, info, sizeof(info));
//        // if (info->flags & MULTIBOOT_INFO_BOOTDEV) {
//            // printf("bootdevice = %04x.%04x\r\n",
//                // info->boot_device >> 16, info->boot_device & 0xFFFF);
//        // }
//        // if (info->flags & MULTIBOOT_INFO_CMDLINE) {
//            // const char* cmdline = (const char*)
//                // mapBootData(dest, (void*)info->cmdline, 512);
//            // printf("cmdline = %s\r\n", cmdline);
//        // }
//        // if (info->flags & MULTIBOOT_INFO_MODS) {
//            // multiboot_module_t* module = (multiboot_module_t*)info->mods_addr;
//            // if (info->mods_count > 0) {
//                // KERNEL->mapBootloaderData(module,
//                    // info->mods_count * sizeof(multiboot_info_t));
//            // }
//            // for (uint32_t i = 0; i < info->mods_count; i++, module++) {
//                // void* start = (void*)module->mod_start;
//                // size_t len = module->mod_end - (uint32_t)start;
//                // KERNEL->mapBootloaderData(start, len);
//                // printf("module @ %p * %u: %s\r\n", start, len, module->cmdline);
//                // //MemoryStream stream(start, len, 0);
//                // //loadElf(stream);
//            // }
//        // }
//        // if (info->flags & MULTIBOOT_INFO_AOUT_SYMS) {
//            // printf("aout syms\r\n");
//        // }
//        // if (info->flags & MULTIBOOT_INFO_ELF_SHDR) {
//            // printf("elf shdr\r\n");
//        // }
//        // if (info->flags & MULTIBOOT_INFO_DRIVE_INFO) {
//            // printf("drive info\r\n");
//        // }
//        // if (info->flags & MULTIBOOT_INFO_CONFIG_TABLE) {
//            // printf("config table\r\n");
//        // }
//        // if (info->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) {
//            // dir->mapBootloaderData((const void*)info->boot_loader_name, PAGESIZE);
//            // printf("bootloader name = %s\r\n",
//                // (const char*)info->boot_loader_name);
//        // }
//        // if (info->flags & MULTIBOOT_INFO_APM_TABLE) {
//            // printf("apm table\r\n");
//        // }
//        // if (info->flags & MULTIBOOT_INFO_VIDEO_INFO) {
//            // printf("video info\r\n");
//        // }
//        /*if (info->flags & MULTIBOOT_INFO_MEM_MAP) {
//            const multiboot_memory_map_t* descr =
//                (const multiboot_memory_map_t*)info->mmap_addr;
//            uint32_t len = info->mmap_length;
//            i386::mapBootloaderData(descr, len);
//            const multiboot_memory_map_t* end =
//                (const multiboot_memory_map_t*)((const char*)descr + len);
//            while (descr < end) {
//                // cannot use 64 bit integers because of a bug in clang, which
//                // forces the creation of code for xmm registers (which are not)
//                // available on 80386
//                uint32_t addrHigh = *(((uint32_t*)&descr->addr) + 1);
//                if (descr->type == MULTIBOOT_MEMORY_AVAILABLE && addrHigh == 0) {
//                    uint32_t addrLow = (uint32_t)descr->addr;
//                    uint32_t lenHigh = *(((uint32_t*)&descr->len) + 1);
//                    uint32_t len = (uint32_t)descr->len;
//                    if (addrLow + len > (uint32_t)&CODE || lenHigh != 0) {
//                        len = (uint32_t)&CODE - addrLow;
//                    }
//                    i386::mapFree((const void*)addrLow, len);
//                }
//                descr = (const multiboot_memory_map_t*)
//                    ((const char*)descr + descr->size + 4);
//            }
//        } else*/ if (info->flags & MULTIBOOT_INFO_MEMORY) {
//            reportFree(nullptr, TRUNC(min(info->mem_lower, 639) * 1024));
//            reportFree((void*)0x100000, TRUNC(min(info->mem_upper * 1024,
//                (uint32_t)&CODE - 0x100000)));
//        } else {
//            printf("No memory information provided by bootloader.\r\n");
//            halt();
//        }
//        AddressSpace::kernel.dump();
//    }
//
//};
//
///**
// * The global descriptor table. The entries are structured as follows:
// *
// * Offset| Description          | Remarks
// * -----:|----------------------|---------------------------------------------
// * 000   | null-entry           | required by the processor specificaton
// * 010   | kernel code segment  | ring 0, execute-only
// * 020   | kernel data segment  | ring 0, read/write
// * 030   | user code segment    | ring 3, callable from ring 0, execute-only
// * 040   | user data segment    | rint 3, read/write
// * 050   | task state for CPU 0 |
// * 060   | task state for CPU 1 |
// * ...   | ...                  |
// *
// * All code and data segments are 4 GByte long and start at the virtual address
// * 0 and use 32 bits mode.
// *
// * Each entry is defined as follows:
// *
// * Bits  | Description
// * -----:|----------------------------------------
// * 0-15  | maximum address in segment, bits 0-15
// * 16-39 | virtual start address, bits 0-23
// * 40-47 | access byte
// * 48-51 | maximum address in segment, bits 16-19
// * 52-55 | flags
// * 56-63 | virtual start address, bits 24-31
// *
// * The flags have the following meanings:
// *
// * Bit | Description
// * ---:|----------------------------------------------------------------------
// *  54 | access using 32 bit registers, code uses 32 bit instructions
// *  55 | the maximum address is defined in 4 K blocks (instead of Bytes)
// *
// * The access bytes for code segments is defined as follows:
// *
// * Bits  | Description
// * ------|---------------------------------------------------------------------
// * 40    | set by the processor, if the segment is accessed
// * 41    | code segment is readable
// * 42    | executable from a lower privelege ring
// * 43    | 1
// * 44    | 1
// * 45-46 | privilege level (0 = kernel, 3 = user)
// * 47    | segment physically present in memory
// *
// * If bit 42 is set, the code may be called or jumped to from a lower privelege
// * level. E.g., ring 3 code can be far-called from ring 0. If bit 42 is zero,
// * the code can only be executed from another segment with the same privilege
// * level.
// *
// * The access bytes for data segments is defined as follows:
// *
// * Bits  | Description
// * ------|---------------------------------------------------------------------
// * 40    | set by the processor, if the segment is accessed
// * 41    | data segment is writable
// * 42    | segment grows down, i.e. offset must be greater than limit
// * 43    | 9
// * 44    | 1
// * 45-46 | privilege level (0 = kernel, 3 = user)
// * 47    | segment physically present in memory
// */
//uint64_t __attribute__((aligned(8))) GDT[] = {
//    0x0000000000000000,     // must be 0 due to intel specification
//    0x00CF98000000FFFF,     // kernel code
//    0x00CF92000000FFFF,     // kernel data
//    0x00CFFC000000FFFF,     // user code
//    0x00CFF2000000FFFF,     // user data
//};
//
///**
// * The interrupt descriptor table. The table is corrected by the
// * @ref AddressSpace::init function, so it can be interpreted by the CPU.
// * Initially, the bits 16...31 are swapped with the bits 32...47, so the linear
// * address of the interrupt handler ist stored in bits 0...31. Therefore, the
// * table can be filled by the compiler using the addresses of the handler.
// *
// * Each entry is defined as follows:
// *
// * Bits  | Description
// * -----:|-----------------------------------------------
// * 0-31  | address of the interrupt handler
// * 32-39 | unused (should be 0)
// * 40-42 | gate type, refer to table below for details
// * 43    | use 32 bit addresses
// * 44    | storage segment (0 for interrupt gates)
// * 45-46 | privilege level for caller
// * 47    | present
// * 48-63 | segment descriptor (8 for kernel code segment)
// *
// * The following gate taypes are defined:
// *
// * Value | Description
// * -----:|-------------------------------------------
// * 1     | available task state segment
// * 2     | local descriptor table (16 bits only)
// * 3     | busy task state segment
// * 4     | call gate
// * 5     | task gate
// * 6     | interrupt gate
// * 7     | trap gate
// *
// * An interrupt pushes the following values on the stack:
// * - SS:ESP, if the privielege level changes
// * - EFLAGS
// * - CS:EIP
// * - error code
// */
//extern uint64_t IDT[32];
//
///**
// * Loads the global descriptor table from @ref GDT. The code segment register
// * is initialized to the kernel code and the instruction pointer is updated to
// * the kernel's virtual address. All other segment registers are set to the
// * kernel's data segment. The stack is adjusted in a way, that the instruction
// * pointer, the stack pointer, and the base pointer will use the virtual kernel
// * addresses after a return from this function.
// */
//static inline void loadGDT() {
//    // load the global descriptor table
//    asm(
//        "pushl  %%ebx;"
//        "pushw  %%ax;"
//        "lgdt   (%%esp);" : :
//        "a"(sizeof(i386::GDT) - 1), "b"(&i386::GDT));
//    // initialize segment registers and set instruction pointer to the new
//    // kernel area
//    asm(
//        "ljmpl  $010, $.loadCs;"
//        ".loadCs:;"
//        "movw   $020, %%ax;"
//        "movw   %%ax, %%ds;"
//        "movw   %%ax, %%es;"
//        "movw   %%ax, %%fs;"
//        "movw   %%ax, %%gs;"
//        "movw   %%ax, %%ss;"
//        "addl   $6, %%esp;" : :);
//}
//
///**
// * Loads the interrupt descriptor table from @ref IDT. Therefore, its contents
// * are adjusted.
// */
//void loadIDT() {
//    // adjust the interrupt descriptor table
//    asm(
//        "movl   %%eax, %%esi;"
//        "movl   %%eax, %%edi;"
//        "cld;"
//        ".initIdt:;"
//        "lodsl;"
//        "movl   %%eax, %%ebx;"
//        "lodsl;"
//        "xchgw  %%bx, %%ax;"
//        "stosl;"
//        "movl   %%ebx, %%eax;"
//        "stosl;"
//        "loop   .initIdt;" : :
//        "a"(&i386::IDT), "c"(ARRAYSIZE(i386::IDT)));
//    // load the interrupt descriptor table
//    asm(
//        "pushl  %%ebx;"
//        "pushw  %%ax;"
//        "lidt   (%%esp);"
//        "addl   $6, %%esp;" : :
//        "a"(sizeof(i386::IDT) - 1), "b"(&i386::IDT));
//}
//
//}
//
//using namespace i386;
//
//#ifdef VERBOSE
//void AddressSpace::dump() {
//   printf("===========================================\r\n");
//    printf("PagingDirectory @ %p\r\n", this);
//    const char* start = nullptr;
//    do {
//        uint32_t startEntry = ((AddressSpaceImpl*)this)->getEntry(start);
//        if (!ISUNUSED(startEntry)) {
//            const char* end = start;
//            uint32_t startAttr = ATTR(startEntry);
//            uint32_t endEntry;
//            const char* phys = (char*)TRUNC(startEntry);
//            do {
//                end += MEMPAGE_SIZE;
//                phys += MEMPAGE_SIZE;
//                endEntry = ((AddressSpaceImpl*)this)->getEntry(end);
//            } while (end != nullptr && (uint32_t)phys == TRUNC(endEntry) &&
//                startAttr == ATTR(endEntry));
//            char attribs[10];
//            char* current = attribs;
//            *current++ = startEntry & i386::PA_GLOBAL ? 'G' : '-';
//            *current++ = startEntry & i386::PA_4MBYTE ? 'L' : '-';
//            *current++ = startEntry & i386::PA_DIRTY ? 'D' : '-';
//            *current++ = startEntry & i386::PA_ACCESSED ? 'A' : '-';
//            *current++ = startEntry & i386::PA_NOCACHE ? '-' : 'C';
//            *current++ = startEntry & i386::PA_WRITETHRU ? 'T' : '-';
//            *current++ = startEntry & i386::PA_RING0 ? 'U' : 'K';
//            *current++ = startEntry & i386::PA_WRITABLE ? 'W' : 'R';
//            *current++ = startEntry & i386::PA_PRESENT ? 'P' : '-';
//            *current = 0;
//            uint32_t userAttrs = ATTR(startEntry) >> 9;
//            printf("%p...%p -> %08x %s %x\r\n", start, end - 1,
//                TRUNC(startEntry), attribs, userAttrs);
//            start = end;
//        } else {
//            start += MEMPAGE_SIZE;
//        }
//    } while (start != nullptr);
//    printf("===========================================\r\n");
//}
//#endif
//
//// AddressSpace* AddressSpace::create() {
//    // printf("create not implemented");
//    // halt();
//// //    return (AddressSpace*)KERNEL->allocate(PA_USER | PA_WRITABLE);
//// }
//
//bool AddressSpace::map(const void* virt, const void* phys, size_t size,
//    bool write, bool user) {
//    if (ATTR(virt) != 0 || ATTR(phys) != 0 || ATTR(size) != 0 ||
//        size == 0 || ((uint32_t)virt + size < (uint32_t)virt) ||
//        ((uint32_t)phys + size < (uint32_t)phys)) {
//        errno = EINVAL;
//        return false;
//    }
//    bool result = true;
//    uint32_t attr = i386::PA_PRESENT;
//    if (write) {
//        attr |= i386::PA_WRITABLE;
//    }
//    if (user) {
//        attr |= i386::PA_RING0;
//    }
//    // The virtually mapped memory is mapped as globally available. It will be
//    // mapped by all threads to the same memory area to be accessible from all
//    // threads. Marking it as globally available leads into increased access
//    // speed, as the corresponding mapping entries don't need to be updated on
//    // task switches.
//    if (virt >= &CODE) {
//        attr |= i386::PA_GLOBAL;
//    }
//    for (; size != 0; size -= MEMPAGE_SIZE) {
//        result &= ((AddressSpaceImpl*)this)->mapPage(virt, phys, attr);
//        virt = (char*)virt + MEMPAGE_SIZE;
//        phys = (char*)phys + MEMPAGE_SIZE;
//    }
//    return result;
//}
//
//// bool AddressSpace::unmap(const void* virt, size_t size) {
//    // printf("unmap not implemented");
//    // halt();
//    // if ((uint32_t)virt + size < (uint32_t)virt || size == 0 ||
//        // ATTR(size) != 0) {
//        // errno = EINVAL;
//        // return false;
//    // }
//    // errno = ESUCCESS;
//    // AddressSpaceImpl* old = (AddressSpaceImpl*)
//        // AddressSpaceImpl::getCurrentAddressSpace();
//    // kernel.load();
//    // pageTable_t& dir = *(pageTable_t*)getVirtualAddress(&old->contents);
//    // for (; size != 0; size -= PAGESIZE) {
//        // uint32_t dirEntry = dir[(uint32_t)virt >> 22];
//        // if (!ISUNUSED(dirEntry)) {
//            // pageTable_t& table = *(pageTable_t*)
//                // getVirtualAddress((void*)TRUNC(dirEntry));
//            // table[((uint32_t)virt >> 12) & 1023] = 0;
//        // }
//        // virt = (const void*)((char*)virt + PAGESIZE);
//    // }
//    // AddressSpaceImpl::setCurrentAddressSpace(old);
//    // return true;
//// }

//void AddressSpace::init(void* data) {
//    AddressSpaceImpl* dir = AddressSpaceImpl::adjustAddresses(&kernel);
//    dir->mapKernel();
//    dir->load();
//    enablePaging();
//    loadGDT();
//    AddressSpaceImpl::adjustStack();
//    loadIDT();
//    dir->unmapAll();
//    AddressSpaceImpl::evaluateMultibootData((multiboot_info_t*)data);
//}

//void* AddressSpace::getPhysicalAddress(const void* virt) {
//    const uint32_t delta = (uint32_t)(&CODE - &PHYS);
//    if (!isPagingEnabled()) {
//        return (void*)virt;
//    }
//    if (virt >= &CODE) {
//        return (char*)virt - (uint32_t)(&CODE - &PHYS);
//    }
//    uint32_t dir = ((AddressSpaceImpl*)this)->contents[DIRINDEX(virt)];
//    if (ISUNUSED(dir)) {
//        return INVALID_PTR;
//    }
//    pageTable_t& table = *(pageTable_t*)TRUNC(dir);
//    uint32_t result = table[TABLEINDEX(virt)];
//    if (ISUNUSED(result)) {
//        return INVALID_PTR;
//    } else {
//        return (void*)result;
//    }
//}
//
//void AddressSpace::load() {
//    asm(
//        "movl   %%eax, %%cr3;" : :
//        "a"(getPhysicalAddress(this)));
//}

void* AddressSpace::getPhysicalAddressImpl(const void* virtAddr) {
	if (virtAddr >= &kernelAddr) {
		size_t delta = &kernelAddr - &bootAddr;
		return (void*)((const char*)virtAddr - delta);
	}
	if (!isPagingEnabled()) {
		return (void*)virtAddr;
	}
	return invalidPtr<void>();
}

bool AddressSpace::isPagingEnabled() {
    uint32_t result;
    asm(
        "movl   %%cr0, %%eax;" :
        "=a"(result) : );
    return (result & 0x80000000) != 0;
}
