#define ASM_FILE
#include <multiboot/multiboot.h>
#define MULTIBOOT_FLAGS         (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)

.section .phys
/*
 * The MultiBoot header.
 */
    .align  4
    .long   MULTIBOOT_HEADER_MAGIC;
    .long   MULTIBOOT_FLAGS;
    .long   - MULTIBOOT_FLAGS - MULTIBOOT_HEADER_MAGIC;

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
    cmpl    $MULTIBOOT_BOOTLOADER_MAGIC, %eax
    jnz     halt
    // initialize stack
    movl    $_stack.end, %esp
    subl    $KERNEL_OFFSET, %esp
    // call i386::PagingDirectory::initMemMap(%ebx)
    pushl   %ebx
    call    _ZN4i38615PagingDirectory4initEP14multiboot_info
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
    // loop infinetly
.size _start, . - _start
.type halt, @function
.globl halt
halt:
    cli
    hlt
    jmp     halt
.size halt, . - halt

/*
 * The stack for the kernel (4 KByte).
 */
.section .bss
_stack:
.skip 4096
.size _stack, . - _stack
_stack.end:

/*
 * Two empty page tables. The first table maps the memory areas used by the
 * kernel 1:1 into physical memory. The other table maps the kernel to the
 * uppermost 4 MByte address range.
 */
FirstPageTable:
.skip 4096
LastPageTable:
.skip 4096

.section .data
.align 4096
_ZN4i38621kernelPagingDirectoryE:
.globl _ZN4i38621kernelPagingDirectoryE
    .int    FirstPageTable + 0xE03
    .skip(4088)
    .int    LastPageTable + 0xE07

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
