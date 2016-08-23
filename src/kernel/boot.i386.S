#define ASM_FILE
#include <multiboot/multiboot.h>
#define MULTIBOOT_FLAGS         (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)

// Physically present in memory.
#define PA_PRESENT              1
// Write access allowed.
#define PA_WRITABLE             2
// Accessible by user code.
#define PA_RING0                4
// Write through caching enabled.
#define PA_WRITETHRU            8
// Caching disabled.
#define PA_NOCACHE              16
// Set by read operation on memory in page.
#define PA_ACCESSED             32
// Set by write operation on memory in page.
#define PA_DIRTY                64
// Directory entry discrables 4Mbyte page instead of page table.
#define PA_4MBYTE               128
// Entry is used globally, i.e. it is not updated, when a new table is loaded.
#define PA_GLOBAL               256

.section .boot
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
    // save offset between virtual and physical addresses for later use
    movl    $KERNEL_OFFSET, %edx
    // init 1:1 paging for the kenel address range
    cld
    movl    $(bootAddr + PA_PRESENT), %eax
    movl    %eax, %edi
    shrl    $10, %edi
    addl    $FirstPageTable, %edi
    subl    %edx, %edi
    cld
    xor     %ecx, %ecx
_start.initFirstPageTableReadOnly:
    stosl
    inc     %ecx
    addl    $4096, %eax
    cmpl    $physReadWrite, %eax
    jb      _start.initFirstPageTableReadOnly
    orl     $PA_WRITABLE, %eax
_start.initFirstPageTableReadWrite:
    stosl
    inc     %ecx
    addl    $4096, %eax
    cmpl    $physEnd, %eax
    jb      _start.initFirstPageTableReadWrite
    // init paging to virtual kernel address range
    movl    $bootAddr, %esi
    shrl    $10, %esi
    addl    $FirstPageTable, %esi
    subl    %edx, %esi
    movl    $LastPageTable, %edi
    subl    %edx, %edi
_start.initVirtualPageTable:
    lodsl
    xorl    $PA_GLOBAL, %eax
    stosl
    loop    _start.initVirtualPageTable
    // adjust addresses in paging directory
    movl    $_ZN12AddressSpace6kernelE, %edi
    subl    %edx, %edi
    movl    %edi, %esi
    movl    $1024, %ecx
_start.adjustPagingDirectory:
    lodsl
    orl     %eax, %eax
    jz      _start.dontAdjustPagingEntry
    subl    $KERNEL_OFFSET, %eax
_start.dontAdjustPagingEntry:
    stosl
    loop    _start.adjustPagingDirectory
    // load paging directory
    movl    $_ZN12AddressSpace6kernelE, %eax
    subl    $KERNEL_OFFSET, %eax
    movl    %eax, %cr3
    // enable paging
    movl    %cr0, %eax
    orl     $0x80010000, %eax
    movl    %eax, %cr0
    // initialize stack
    movl    $_stack.end, %esp
    movl    %esp, %ebp
    // load the global descriptor table
    movl    $GDT, %eax
    pushl   %eax
    movl    $GDT.end, %eax
    subl    $GDT + 1, %eax
    pushw   %ax
    lgdt    (%esp)
    addl    $6, %esp
    // initialize segment registers and set instruction pointer to the new
    // kernel area
    ljmpl   $010, $_start.loadCs
_start.loadCs:
    movw    $020, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss
    // call kernel main((multiboot_info_t*)%ebx)
    pushl   %ebx
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
.align(4096)
_stack:
.skip 4096
.size _stack, . - _stack
_stack.end:

/*
 * Two empty page tables. The first table maps the memory areas used by the
 * kernel 1:1 into physical memory. The other table maps the kernel to the
 * uppermost 4 MByte address range.
 */
.align 4096
FirstPageTable:
    .skip 4096
LastPageTable:
    .skip 4096

.section .data
.align 4096
_ZN12AddressSpace6kernelE:
.globl _ZN12AddressSpace6kernelE
    .int    FirstPageTable + 0x003
    .skip(4088)
    .int    LastPageTable + 0x107

/*
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
 * 43    | 0
 * 44    | 1
 * 45-46 | privilege level (0 = kernel, 3 = user)
 * 47    | segment physically present in memory
 */
.align 8
GDT:
    .quad   0x0000000000000000      // must be 0 due to intel specification
    .quad   0x00CF98000000FFFF      // kernel code
    .quad   0x00CF92000000FFFF      // kernel data
    .quad   0x00CFFC000000FFFF      // user code
    .quad   0x00CFF2000000FFFF      // user data
GDT.end:
.size GDT, . - GDT


.section .text
_handleException:
    pushw   $0
    pushw   %ds
    pushw   $0
    pushw   %es
    pushw   $0
    pushw   %fs
    pushw   $0
    pushw   %gs
    pushal
    movl    56(%esp), %eax
    pushl   %eax
    call    handleException
    popal
    popw    %gs
    addl    $2, %esp
    popw    %fs
    addl    $2, %esp
    popw    %es
    addl    $2, %esp
    popw    %ds
    addl    $10, %esp
    iret

.section .data
.align(8)
.globl _ZN4i3863IDTE
_ZN4i3863IDTE:

.macro exceptionHandler number, hasErrorCode
.type _exception\number, @function
.section .phys
.globl _exception\number
_exception\number:
.ifeq \hasErrorCode
    pushl   $0
.endif
    pushl   $\number
    jmp     _handleException
.size _exception\number, . - _exception\number
.section .data
    .int    _exception\number
    .int    0x00088E00
.endm

/*
exceptionHandler 0, 0       // Divide by 0
exceptionHandler 1, 0       // Debugger Exception
exceptionHandler 2, 0       // Nonmaskable Interrupt
exceptionHandler 3, 0       // Breakpoint
exceptionHandler 4, 0       // Overflow
exceptionHandler 5, 0       // Bounds Check
exceptionHandler 6, 0       // Invalid Opcode
exceptionHandler 7, 0       // Coprocessor not Available
exceptionHandler 8, 1       // Double Fault
exceptionHandler 9, 0       // Coprocessor Segment Overrun
exceptionHandler 10, 1      // Invalid Task State Segment
exceptionHandler 11, 1      // Coprocessor Segment not Present
exceptionHandler 12, 1      // Stack Exception
exceptionHandler 13, 1      // General Protection Fault
exceptionHandler 14, 1      // Page Fault
exceptionHandler 15, 0      // Coprocessor Error
exceptionHandler 16, 0      // Alignment Check
exceptionHandler 17, 0      // Machine Check
exceptionHandler 18, 0      // SIMD Exception
exceptionHandler 19, 0
exceptionHandler 20, 0
exceptionHandler 21, 0
exceptionHandler 22, 0
exceptionHandler 23, 0
exceptionHandler 24, 0
exceptionHandler 25, 0
exceptionHandler 26, 0
exceptionHandler 27, 0
exceptionHandler 28, 0
exceptionHandler 29, 0
exceptionHandler 30, 0
exceptionHandler 31, 0
*/

.section .data
_ZN4i3863IDTE.end:
.size _ZN4i3863IDTE, . - _ZN4i3863IDTE.end

.end
