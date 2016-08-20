.section .boot
.globl _start
_start:
.size _start, . - _start


    # Loop over this forevermore
.globl halt
halt:
    b       halt
.size _halt, . - _halt
