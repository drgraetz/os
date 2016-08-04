# Dr.Grätz OS - The Secure Operating System

Execute src/build.py to build. Pass --help to see possible build parameters.

The following directory structure is used:
- src: all source codes including configuration files and the build script
- LICENSE: the license under which Dr.Grätz OS is published
- README.md: this file

The following directories are created by the build process:
- log: all log files
- obj: all compiled object files, sorted by platform
- bin: all generated binaries, sorted by platform
- tools: all 3rd-party tools

The build script requires, that the following software packages are available
on the system path:
- python3: Python interpreter, required to run src/build.py
- clang: C++ compiler, required to compile the source files
- doxygen (optional): creates the project documentation
- qemu (optional): virtual machine on which the tests are run
