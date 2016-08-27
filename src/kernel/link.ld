ENTRY(_start)

KERNEL_OFFSET = kernelAddr - bootAddr;

SECTIONS {
    . = kernelAddr;
    /* code */
    .text : AT(bootAddr) {
    _ZN12AddressSpace13PHYSICAL_ADDRE = . - KERNEL_OFFSET;
    _ZN12AddressSpace11KERNEL_CODEE = .;
        /* boot code */
        *(.boot)
        /* all other code */
        *(.text*)
        /* GCC exception handler */
        *(.eh_frame)
    }
    /* readonly data */
    .rodata : AT(ADDR(.rodata) - KERNEL_OFFSET) {
    _ZN12AddressSpace16KERNEL_READ_ONLYE = .;
    physReadOnly = . - KERNEL_OFFSET;
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
    /* uninitialized data, e.g. stack, uninitialized variables */
    .bss : AT(ADDR(.bss) - KERNEL_OFFSET) {
    _ZN12AddressSpace17KERNEL_READ_WRITEE = .;
    physReadWrite = . - KERNEL_OFFSET;
        *(.bss)
    }
    /* initialized, writable data */
    .data : AT(ADDR(.data) - KERNEL_OFFSET) {
        *(.data)
    _ZN12AddressSpace10KERNEL_ENDE = ALIGN(pageSize);
    }
    /* discard all other segments */
    /DISCARD/ : {
    	*(*)
    }
    _ZN12AddressSpace17KERNEL_END = .;
}
