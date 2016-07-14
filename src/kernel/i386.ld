ENTRY(_start)
/* DESC = intel 80386 and compatibles */
/* CPU = i386 */
OUTPUT_ARCH(i386)

PHYS_ADDR = 0x00100000;
VIRT_ADDR = 0xFFF00000;

KERNEL_OFFSET = VIRT_ADDR - PHYS_ADDR;

SECTIONS {
    . = PHYS_ADDR;
    .text : {
        *(.phys)
        *(.text)
    }
    .bss : ALIGN(4K) {
        *(.bss)
    }
}
