// r15 -> should begin execution at 0x8000.
// r0 -> 0x00000000
// r1 -> 0x00000C42
// r2 -> 0x00000100 - start of ATAGS
// preserve these registers as argument for kernel_main
.section .boot
.globl _start
_start:
	// initialize stack
    ldr    sp, =stack.end
	// call kmain
	blx    kmain
    // loop infinetly
.size _start, . - _start
.globl halt
halt:
    wfi
    b       halt
.size _halt, . - halt

.section .rodata
.align 4
stack:
	.skip(4096)
stack.end:
.size stack, . - stack