# Dr. Grätz OS - The Secure Operating System

## The Build Process

### Build Instructions

Execute _src/build.py_ to build. Pass --help to see possible build parameters.
The build has been executed successfully on cygwin but should also run on any
linux platform.

### Dependencies

The build script requires, that the following software packages are available
on the system path:
- _python3_: Python interpreter, required to run src/build.py
- _cmake_: CMake tool chain, required to build clang
- _diffutils_: required by the make process of binutils
- _doxygen_ (optional): creates the project documentation
- _lxml_: XML library for Python, required to run src/build.py
- _make_: make tool chain, required to build 3rd party tools
- _qemu_ (optional): virtual machine on which the tests are run

The build process has been successfully tested on a Cygwin environment.

### Directory Structure

- _src_: source codes including configuration files and the build script
- _src/build.py_: Python build script
- _src/buildinfo.xml_: build descriptor
- _src/buildinfo.dtd_: schema for the build descriptor
- _src/doxygen.config_: configuration for doxygen
- _src/ext_: required external sources
- _src/include_: commonly used include files
- _src/kernel_: kernel source code
- _LICENSE_: license under which Dr.Grätz OS is published
- _README_.md: this file

### Files Created During the Build Process

The following directories are created by the build process:
- _bin_: all generated binaries, sorted by platform
- _doc_ (optional): the doxygen documentation
- _logs_: all log files
- _obj_: all compiled object files, sorted by platform
- _specs_: all specifications required for development, downloaded from various
  internet pages
- _tools_: all 3rd-party tools

## Conventions

### File Names

#### Linker Scripts

Linker scripts are named _linker.ld_

#### Architecture-Dependent Source Code Files

The file extension of architecture-specific source code files is preceeded by
a dot and the name of the extension, e.g. boot.i386.S, or
AddressSpace.i386.cpp.

## Coding

- Do not use macros unless necessary. Rather use function templates.
- Use expressive function names.

## The Boot Process

Dr. Grätz OS is loaded using a boot loader, e.g.
[GRUB](https://gnu.org/software/grub) or
[UBOOT](http://www.denx.de/wiki/U-Boot). Therefore, the kernel is linked into
an elf-file with a physical loading address typically used by the bootloader
and the target virtual address of the kernel, which differs from the physical
address. In a later phase direct booting of the kernel will be supported. For
this, the elf file will have to be dumped to a plain binary file using the
architecture-specific objdump tool.

The kernel's entry point is the _start() function. Refer to its documentation
for details on the boot process.

