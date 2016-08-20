.section .boot
.globl _start
_start:
.size _start, . - _start
	// initialize stack
	mov		sp, #0x8000
    // loop infinetly
.globl halt
halt:
    b       halt
.size _halt, . - _halt
