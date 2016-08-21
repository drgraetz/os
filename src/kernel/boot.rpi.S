.section .boot
.globl _start
_start:
.size _start, . - _start
	// initialize stack
	ldr		sp, =stack.end
    // loop infinetly
.globl __cxa_pure_virtual
__cxa_pure_virtual:
.globl halt
halt:
    b       halt
.size _halt, . - _halt

.section .rodata
.align 4
stack:
	.skip(4096)
stack.end:
.size stack, . - stack
