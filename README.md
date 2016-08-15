Dr.Grätz OS - The Secure Operating System
=========================================

Build Instructions
------------------

Execute _src/build.py_ to build. Pass --help to see possible build parameters. The build has been executed successfully on cygwin but should also run on any linux platform.

Dependencies
------------

The build script requires, that the following software packages are available
on the system path:
- _python3_: Python interpreter, required to run src/build.py
- _clang_: C++ compiler, required to compile the source files
- _doxygen_ (optional): creates the project documentation
- _qemu_ (optional): virtual machine on which the tests are run

Directory Structure
-------------------

- _src_: source codes including configuration files and the build script
- _src/ext_: required external sources
- _src/include_: commonly used include files
- _src/kernel_: kernel source code
- _LICENSE_: license under which Dr.Grätz OS is published
- _README_.md: this file

Files Created During the Build Process
--------------------------------------

The following directories are created by the build process:
- _bin_: all generated binaries, sorted by platform
- _doc_ (optional): the doxygen documentation
- _logs_: all log files
- _obj_: all compiled object files, sorted by platform
- _tools_: all 3rd-party tools
