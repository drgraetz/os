ENTRY(_start)

KERNEL_OFFSET = kernelAddr - bootAddr;

SECTIONS {
    . = kernelAddr;
    /* code */
    .text : AT(bootAddr) {
        /* boot code */
        *(.boot)
        /* all other code */
        *(.text)
        /* GCC exception handler */
        *(.eh_frame)
    }
    /* uninitialized data, e.g. stack, uninitialized variables */
    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_OFFSET) {
        /*boot.*.o(.bss)*/
        *(.bss)
    }
    /* initialized, writable data */
    .data : AT(ADDR(.data) - KERNEL_OFFSET) {
    	/*boot.*.o(.data)*/
        *(.data)
    }
    /* readonly data */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_OFFSET) {
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
    	*(*)
    }
}
