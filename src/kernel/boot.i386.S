#define MULTIBOOT_ID            0x1BADB002
#define MULTIBOOT_FLAGS         3
#define MULTIBOOT_ACKNOWLEDGE   0x2BADB002
#define KERNEL_OFFSET           0xFFE00000

.section .phys
/*
 * The MultiBoot header.
 */
    .align  4
    .long   MULTIBOOT_ID;
    .long   MULTIBOOT_FLAGS;
    .long   - MULTIBOOT_FLAGS - MULTIBOOT_ID;

/*
 * \fn void _start();
 *
 * The entry function to the kernel. It is assumed, that the kernel is loaded
 * by a multiboot compliant bootloader, such as GRUB. I.e. the following
 * conditions are met:
 * - the processor is in protected mode
 * - the kernel is loaded at 1 MByte
 * - the CS, DS segments are initialized to 4 GByte and mapped 1:1 to physical
 *   memory
 * - 32 bit adressing is used for code and data
 */
.type _start, @function
.globl _start
_start:
    // if not started by a multiboot compliant header, loop infinetly
    cli
    cmpl    $MULTIBOOT_ACKNOWLEDGE, %eax
    jnz     _start.infinty
    // enable paging
    movl    $PagingDirectory - KERNEL_OFFSET, %eax
    movl    %eax, %cr3
    movl    %cr0, %eax
    orl     $0x80010000, %eax
    movl    %eax, %cr0
    // initialize stack
    movl    $_stack.end, %esp
    // load the global descriptor table and initialize segment registers
    pushl   $_gdt
    pushw   $(_gdt.end - _gdt - 1)
    lgdt    (%esp)
    addl    $6, %esp
    ljmpl   $010, $_start.load_cs
_start.load_cs:
    movw    $020, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss
    // call kernel main
    call    kmain
    // loop infintely
_start.infinty:
    cli
    hlt
    jmp     _start.infinty
.size _start, . - _start

/*
 * The stack for the kernel (4 KByte).
 */
.section .bss
_stack:
.skip 4096
.size _stack, . - _stack
_stack.end:

.section .data
/**
 * The paging directory and the used page tables. The page directory points to
 * page tables. The upper 10 bits of an address locate a page table in the page
 * directory.
 *
 * If bit 6 of the corresponding page directory entry is 0, the next 10 bits of
 * the address locate a page in the page table. The least significant 12 bits
 * of an address determine an offset within this page.
 *
 * If bit 6 of a page directory entry is 1, it describes a 4 Mbyte page. The
 * least significant 22 bits of an address determine an offset within this
 * page.
 *
 * Each entry of the paging directory or a paging table is built as follows:
 *
 * Bits  | Used In | Description
 * ------|---------|--------------------------------------------------------
 * 12-32 | both    | physical address of the memory page aligned to 4K
 * 9-11  | both    | user flags, can be used by the operating system
 * 8     | table   | global, will not be updated when new dir is loaded
 * 7     | dir     | 4 MByte page is described instead of page table
 * 6     | table   | dirty, set by processor on write access to page
 * 5     | table   | accessed, set by processor on read access to page
 * 4     | both    | cache disabled
 * 3     | both    | write through caching enabled
 * 2     | both    | user (i.e. non ring 0) can access page
 * 1     | both    | write access allowed
 * 0     | both    | present in physical memory
 *
 * Unused bits should remain 0.
 *
 * Global pages will not be updated, if a new paging directory is loaded to 
 * CR3. In Grätz.OS it is used for the kernel memory, as it is mapped to the
 * same location in all memory maps.
 */
.align 4096
FirstPageTable:
    // no access to the first 736 KByte of memory
    .skip(1024)
    // 1:1 map for 1 Mbyte of kernel memory
    .int    0x00100003, 0x00101003, 0x00102003, 0x00103003
    .int    0x00104003, 0x00105003, 0x00106003, 0x00107003
    .int    0x00108003, 0x00109003, 0x0010A003, 0x0010B003
    .int    0x0010C003, 0x0010D003, 0x0010E003, 0x0010F003
    .int    0x00110003, 0x00111003, 0x00112003, 0x00113003
    .int    0x00114003, 0x00115003, 0x00116003, 0x00117003
    .int    0x00118003, 0x00119003, 0x0011A003, 0x0011B003
    .int    0x0011C003, 0x0011D003, 0x0011E003, 0x0011F003
    .int    0x00120003, 0x00121003, 0x00122003, 0x00123003
    .int    0x00124003, 0x00125003, 0x00126003, 0x00127003
    .int    0x00128003, 0x00129003, 0x0012A003, 0x0012B003
    .int    0x0012C003, 0x0012D003, 0x0012E003, 0x0012F003
    .int    0x00130003, 0x00131003, 0x00132003, 0x00133003
    .int    0x00134003, 0x00135003, 0x00136003, 0x00137003
    .int    0x00138003, 0x00139003, 0x0013A003, 0x0013B003
    .int    0x0013C003, 0x0013D003, 0x0013E003, 0x0013F003
    .int    0x00140003, 0x00141003, 0x00142003, 0x00143003
    .int    0x00144003, 0x00145003, 0x00146003, 0x00147003
    .int    0x00148003, 0x00149003, 0x0014A003, 0x0014B003
    .int    0x0014C003, 0x0014D003, 0x0014E003, 0x0014F003
    .int    0x00150003, 0x00151003, 0x00152003, 0x00153003
    .int    0x00154003, 0x00155003, 0x00156003, 0x00157003
    .int    0x00158003, 0x00159003, 0x0015A003, 0x0015B003
    .int    0x0015C003, 0x0015D003, 0x0015E003, 0x0015F003
    .int    0x00160003, 0x00161003, 0x00162003, 0x00163003
    .int    0x00164003, 0x00165003, 0x00166003, 0x00167003
    .int    0x00168003, 0x00169003, 0x0016A003, 0x0016B003
    .int    0x0016C003, 0x0016D003, 0x0016E003, 0x0016F003
    .int    0x00170003, 0x00171003, 0x00172003, 0x00173003
    .int    0x00174003, 0x00175003, 0x00176003, 0x00177003
    .int    0x00178003, 0x00179003, 0x0017A003, 0x0017B003
    .int    0x0017C003, 0x0017D003, 0x0017E003, 0x0017F003
    .int    0x00180003, 0x00181003, 0x00182003, 0x00183003
    .int    0x00184003, 0x00185003, 0x00186003, 0x00187003
    .int    0x00188003, 0x00189003, 0x0018A003, 0x0018B003
    .int    0x0018C003, 0x0018D003, 0x0018E003, 0x0018F003
    .int    0x00190003, 0x00191003, 0x00192003, 0x00193003
    .int    0x00194003, 0x00195003, 0x00196003, 0x00197003
    .int    0x00198003, 0x00199003, 0x0019A003, 0x0019B003
    .int    0x0019C003, 0x0019D003, 0x0019E003, 0x0019F003
    .int    0x001A0003, 0x001A1003, 0x001A2003, 0x001A3003
    .int    0x001A4003, 0x001A5003, 0x001A6003, 0x001A7003
    .int    0x001A8003, 0x001A9003, 0x001AA003, 0x001AB003
    .int    0x001AC003, 0x001AD003, 0x001AE003, 0x001AF003
    .int    0x001B0003, 0x001B1003, 0x001B2003, 0x001B3003
    .int    0x001B4003, 0x001B5003, 0x001B6003, 0x001B7003
    .int    0x001B8003, 0x001B9003, 0x001BA003, 0x001BB003
    .int    0x001BC003, 0x001BD003, 0x001BE003, 0x001BF003
    .int    0x001C0003, 0x001C1003, 0x001C2003, 0x001C3003
    .int    0x001C4003, 0x001C5003, 0x001C6003, 0x001C7003
    .int    0x001C8003, 0x001C9003, 0x001CA003, 0x001CB003
    .int    0x001CC003, 0x001CD003, 0x001CE003, 0x001CF003
    .int    0x001D0003, 0x001D1003, 0x001D2003, 0x001D3003
    .int    0x001D4003, 0x001D5003, 0x001D6003, 0x001D7003
    .int    0x001D8003, 0x001D9003, 0x001DA003, 0x001DB003
    .int    0x001DC003, 0x001DD003, 0x001DE003, 0x001DF003
    .int    0x001E0003, 0x001E1003, 0x001E2003, 0x001E3003
    .int    0x001E4003, 0x001E5003, 0x001E6003, 0x001E7003
    .int    0x001E8003, 0x001E9003, 0x001EA003, 0x001EB003
    .int    0x001EC003, 0x001ED003, 0x001EE003, 0x001EF003
    .int    0x001F0003, 0x001F1003, 0x001F2003, 0x001F3003
    .int    0x001F4003, 0x001F5003, 0x001F6003, 0x001F7003
    .int    0x001F8003, 0x001F9003, 0x001FA003, 0x001FB003
    .int    0x001FC003, 0x001FD003, 0x001FE003, 0x001FF003
    // no access to MByte 2 through 3
    .skip(2048)
LastPageTable:
    .skip(3072)
    // map the uppermost MByte to the kernel
    // .text segment: 0x0010000 ... 0x00100FFF
    // accessible by user code, read-only, globally available
    .int    0x00100105
    // .bss, .data segments: 0x00101000 ... 0x00106FFF
    // accessible by the kernel, read/write, globally available
    .int                0x00101103, 0x00102103, 0x00103003
    .int    0x00104103, 0x00105103, 0x00106103
    // .rodata segment: 0x00107000 ... 0x00107FFF
    // accessible by the kernel, read-only, globally available
    .int                                        0x00107101
    // the remaining address space is reserved for future use
    .skip(992)
PagingDirectory:
    .int    FirstPageTable - KERNEL_OFFSET + 0x003
    .skip(4088)
    .int    LastPageTable - KERNEL_OFFSET + 0x107

/**
 * The global descriptor table. The entries are structured as follows:
 *
 * Offset| Description          | Remarks
 * -----:|----------------------|---------------------------------------------
 * 000   | null-entry           | required by the processor specificaton
 * 010   | kernel code segment  | ring 0, execute-only
 * 020   | kernel data segment  | ring 0, read/write
 * 030   | user code segment    | ring 3, callable from ring 0, execute-only
 * 040   | user data segment    | rint 3, read/write
 * 050   | task state for CPU 0 |
 * 060   | task state for CPU 1 |
 * ...   | ...                  |
 *
 * All code and data segments are 4 GByte long and start at the virtual address
 * 0 and use 32 bits mode.
 *
 * Each entry is defined as follows:
 *
 * Bits  | Description
 * -----:|----------------------------------------
 * 0-15  | maximum address in segment, bits 0-15
 * 16-39 | virtual start address, bits 0-23
 * 40-47 | access byte
 * 48-51 | maximum address in segment, bits 16-19
 * 52-55 | flags
 * 56-63 | virtual start address, bits 24-31
 *
 * The flags have the following meanings:
 *
 * Bit | Description
 * ---:|----------------------------------------------------------------------
 *  54 | access using 32 bit registers, code uses 32 bit instructions
 *  55 | the maximum address is defined in 4 K blocks (instead of Bytes)
 *
 * The access bytes for code segments is defined as follows:
 *
 * Bits  | Description
 * ------|---------------------------------------------------------------------
 * 40    | set by the processor, if the segment is accessed
 * 41    | code segment is readable
 * 42    | executable from a lower privelege ring
 * 43    | 1
 * 44    | 1
 * 45-46 | privilege level (0 = kernel, 3 = user)
 * 47    | segment physically present in memory
 *
 * If bit 42 is set, the code may be called or jumped to from a lower privelege
 * level. E.g., ring 3 code can be far-called from ring 0. If bit 42 is zero,
 * the code can only be executed from another segment with the same privilege
 * level.
 *
 * The access bytes for data segments is defined as follows:
 *
 * Bits  | Description
 * ------|---------------------------------------------------------------------
 * 40    | set by the processor, if the segment is accessed
 * 41    | data segment is writable
 * 42    | segment grows down, i.e. offset must be greater than limit
 * 43    | 9
 * 44    | 1
 * 45-46 | privilege level (0 = kernel, 3 = user)
 * 47    | segment physically present in memory
 */
.align(8)
_gdt:
    .quad   0
    .quad   0x00CF98000000FFFF
    .quad   0x00CF92000000FFFF
    .quad   0x00CFFC000000FFFF
    .quad   0x00CFF2000000FFFF
    .quad   0
_gdt.end:
.size gdt, . - _gdt

.end
