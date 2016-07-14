#ifndef __KERNEL_HPP
#define __KERNEL_HPP

/**
 * @file
 * The symbol definitions for the kernel.
 */

/**
 * The entry function to the kernel. This function is implemented in the file
 * boot.*.S, where * stands for the target platform.
 */
void _start() __attribute__((noreturn));

#endif
