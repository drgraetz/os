ENTRY(_start)
/* DESC = intel 80386 and compatibles */
/* CPU = i386 */
OUTPUT_ARCH(i386)

PHYS_ADDR = 0x00100000;
VIRT_ADDR = 0xFFC00000;

KERNEL_OFFSET = VIRT_ADDR - PHYS_ADDR;

SECTIONS {
    . = VIRT_ADDR;
    CODE = .;
    PHYS = . - KERNEL_OFFSET;
    /* code */
    .text : AT(PHYS_ADDR) {
        /* physically addressed code (i.e. boot.i386.S) */
        *(.phys)
        /* virtually addressed code (all other code) */
        *(.text)
        /* GCC exception handler */
        *(.eh_frame)
    }
    /* uninitialized data, e.g. stack, uninitialized variables */
    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_OFFSET) {
    READWRITE = .;
        obj/i386/kernel/boot.i386.o(.bss)
        *(.bss)
    }
    /* initialized, writable data */
    .data : AT(ADDR(.data) - KERNEL_OFFSET) {
        obj/i386/kernel/boot.i386.o(.data)
        *(.data)
    }
    /* readonly data */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_OFFSET) {
    READONLY = .;
        *(.rodata*)
        /* list of module initializers */
        . = ALIGN(4);
        __CTOR_LIST__ = .;
        *(.ctors)
        __CTOR_END__ = .;
        /* list of module deinitializers */
        __DTOR_LIST__ = .;
        *(.dtors)
        __DTOR_END__ = .;
    }
    /* discard all other segments */
    /DISCARD/ : {
    }
}
END = .;
