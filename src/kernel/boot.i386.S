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
    movl    %esp, %ebp
    pushl   %ebx
    // call AddressSpace::init((multiboot_info_t*)%ebx)
    call    _ZN12AddressSpace4initEPv
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
FirstPageTable:
.skip 4096
LastPageTable:
.skip 4096

.section .data
.align 4096
_ZN12AddressSpace6kernelE:
.globl _ZN12AddressSpace6kernelE
    .int    FirstPageTable + 0xE03
    .skip(4088)
    .int    LastPageTable + 0xF07

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

.section .data
_ZN4i3863IDTE.end:
.size _ZN4i3863IDTE, . - _ZN4i3863IDTE.end

.end
