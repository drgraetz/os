#define MULTIBOOT_ID            0x1BADB002
#define MULTIBOOT_FLAGS         3


.section .phys
/*
 * The MultiBoot header.
 */
    .align  4
    .long   MULTIBOOT_ID;
    .long   MULTIBOOT_FLAGS;
    .long   - MULTIBOOT_FLAGS - MULTIBOOT_ID;

/*
 * void _start();
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
    // initialize stack
    movl    $_stack.end, %esp
    // main
    //call    kmain
    // Loop infintely
_start.infinty:
    cli
    hlt
    jmp     _start.infinty
.size _start, . - _start

/*
 * The stack for the kernel (4 KByte).
 */
.bss
.skip 4096
_stack.end:

.end
