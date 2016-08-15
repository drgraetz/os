#!/bin/python3
from macpath import dirname

##
# @package  build
# The contents of the build script for Dr Grätz OS (for details refer to
# @ref build.py).
#
# @file
# The build script for Dr Grätz OS.


##
# A directory.
class _Directory:


    ##
    # Cnstructor.    
    def __init__(self, path: str):
        from os.path import abspath
        assert(isinstance(path, str))
        self.__path = abspath(path)
        self.__parent = None
    
    
    
    ##
    # Returns a sub-directory.
    def get(self, rel_path: str):
        from os.path import join
        path = join(self.__path, rel_path)
        return _Directory(path)
    
    
    ##
    # Static initializer. Must be called prior to any other usage of
    # _Directory.
    @staticmethod
    def __static_init__():
        from logging import info
        from os.path import dirname
        from sys import argv
        _Directory.src = _Directory(dirname(argv[0]))
        base_dir = _Directory.src.parent
        _Directory.logs = base_dir.enforce("logs", True)
        _Directory.logs.__create_log_file()
        info("preparing directories")
        _Directory.obj = base_dir.enforce("obj", _CommandLine.rebuild)
        _Directory.doc = base_dir.enforce("doc", True)
        _Directory.bin = base_dir.enforce("bin", _CommandLine.rebuild)
        _Directory.tool = base_dir.enforce("tools")
    
    
    ##
    # Creates a log file in the "logs" directory named like the executable.
    def __create_log_file(self):
        from logging import getLogger, FileHandler, DEBUG
        from os.path import basename, join, splitext
        from sys import argv
        log_name = splitext(basename(argv[0]))[0] + ".log"
        file = FileHandler(join(_Directory.logs.path, log_name), "w")
        file.setLevel(DEBUG)
        getLogger().addHandler(file)
    ##
    # Enforces the presence of a directory. If not already present, the directory
    # and all required parent directories are created. Raises an exception, in case
    # the directory cannot be created.
    #
    # @return The newly created directory.
    def enforce(self, rel_path: str = ".", clear_contents: bool=False):
        from os import mkdir
        from os.path import dirname, isdir, join
        assert(isinstance(rel_path, str))
        assert(isinstance(clear_contents, bool))
        if rel_path == ".":
            result = self
        else:
            result = _Directory(join(self.__path, rel_path))
        if not isdir(result.path):
            result.parent.enforce()
            mkdir(result.path)
        if clear_contents:
            result.erase_contents()
        return result

 
    ##
    # Returns a list of files within this and all sub-directories. A list of
    # file extensions can be provided. If left to None, all files will be
    # returned.
    def get_files(self, extensions: list=None, include_dirs: bool=False) \
        -> list:
        from os import walk
        from os.path import join, splitext
        assert(isinstance(extensions, list) or extensions is None)
        assert(isinstance(include_dirs, bool))
        result = []
        for root_dir, dirs, files in walk(self.__path):
            if include_dirs:
                files += dirs
            result += [join(root_dir, file) for file in files \
                if extensions is None or splitext(file)[1] in extensions]
        return result
    
    
    ##
    # Returns a list of all header files, i.e. all files with the extensions
    # .h and .hpp
    def get_header_files(self):
        return self.get_files([".h", ".hpp"])
    
    
    ##
    # Returns the parent of this.
    @property
    def parent(self):
        from os.path import dirname
        if self.__parent is None:
            self.__parent = _Directory(dirname(self.__path))
        return self.__parent
    
    ##
    # Return the path to this.
    @property
    def path(self) -> str:
        return self.__path
    
    
    ##
    # Erases all contents of this except for a list of files which shall not be
    # deleted. The erase machanisms distingushes between symbol links (which are
    # unlinked), directories (which are recursively deleted), and files (which are
    # removed). Raises an exception, if a file, which is not explicitly excluded,
    # cannot be erased.
    def erase_contents(self, keep_files: list=[]) -> None:
        from os import remove, rmdir, unlink, walk
        from os.path import isdir, islink, join
        for file in reversed(self.get_files(None, True)):
            if islink(file):
                unlink(file)
            elif isdir(file):
                rmdir(file)
            else:
                remove(file)
     
     
##
# A dictionary, that cannot be changed. It wraps an underlying dictionary.
# However, the underlying dictionary can be changed and these changes will
# affect this.
class _ReadOnlyDict:
    ## Constructor wrapping a read only dictionary around a base dictionary.
    def __init__(self, base: dict):
        assert(isinstance(base, dict))
        self.__base = base
    
    
    ## Returns an item by its index.
    def __getitem__(self, key):
        return self.__base[key]
    
    
    ## Returns an iterator on this.
    def __iter__(self):
        return self.__base.__iter__()
    
    
    ## Checks, wether an item is contained in this.
    def __contains__(self, item):
        return item in self.__base
    
    
    ## Returns a list of all values.
    def values(self):
        return self.__base.values()
    
    
    ## Returns a dict_keys of all keys
    def keys(self):
        return self.__base.keys()


##
# A list, that cannot be changed. It wraps an underlying list.
# However, the underlying list can be changed and these changes
# will effect this.
class _ReadOnlyList:
    
    
    ##
    # Constructor.
    def __init__(self, base: list):
        assert(isinstance(base, list))
        self.__base = base
    
    
    ##
    # Creates a _ReadOnlyList containing all elements of this
    # and the elements of the other list.
    def __add__(self, other: list):
        assert(isinstance(other, (list, _ReadOnlyList)))
        if isinstance(other, _ReadOnlyList):
            return _ReadOnlyList(self.__base + other.__base)
        return _ReadOnlyList(self.__base + other)
    
    
    ##
    # Returns an iterator over all elements.
    def __iter__(self):
        return self.__base.__iter__()
    
    
# ##
# # A temporary file or directory. It is typically used within a with
# # statement. It will clean itself up in case of an exception and will
# # prevail on a given target path on normal termination.
# class _TempFile:
#    
#    def __init__(self, path: str, dir: bool = False):
#       from os.path import dirname, isdir, isfile
#       from tempfile import mkdtemp, mkstemp
#       assert(isinstance(path, str))
#       assert(isinstance(dir, bool))
#       self.__path = path
#       _enforce_dir(dirname(path))
#       self.__tmp_handle = None
#       self.__tmp_path = None
#       if dir:
#          if not isdir(self.__path):
#             self.__tmp_path = mkdtemp()
#       else:
#          if not isfile(self.__path):
#             self.__tmp_handle, self.__tmp_path = mkstemp()
#    
#    @property
#    def exists(self) -> bool:
#       from os.path import exists
#       return exists(self.__path)
#    
#    @property
#    def path(self) -> str:
#       return self.__path
#    
#    def __close(self) -> None:
#       from os import close
#       if self.__tmp_handle is not None:
#          close(self.__tmp_handle)
#          self.__tmp_handle = None
#    
#    def close(self) -> None:
#       from os import rename
#       self.__close()
#       if self.__tmp_path is not None:
#          rename(self.__tmp_path, self.__path)
#          self.__tmp_path = None
#    
#    def cancel(self) -> None:
#       self.__close()
#       if self.__tmp_path is not None:
#          _erase(self.__tmp_path)
#          self.__tmp_path = None
#    
#    def write(self, data) -> None:
#       from os import write
#       write(self.__tmp_handle, data)
#    
#    def __del__(self) -> None:
#       self.close()
#    
#    def __enter__(self):
#       return self
#    
#    def __exit__(self, exc_type, exc_value, traceback) -> None:
#       if exc_value is None:
#          self.close()
#       else:
#          self.cancel()
# 
# 
# ##
# # Annotation used for annotationg build targets.
# class _Target:
#    __all = {}
#    __invoked = []
#    all = _ReadOnlyDict(__all)
#    
#    
#    ##
#    # Tracks the execution of build tasks. This function is invoked instead of
#    # the build task function. It will invoke the build task if (and only if)
#    # it has not been previously executed.
#    def __call__(self):
#       if self not in _Target.__invoked:
#          _Target.__invoked.append(self)
#          self.__function()
#    
#    
#    ##
#    # Default constructor.
#    def __init__(self, annotated_element):
#       from inspect import isfunction
#       if not isfunction(annotated_element) or \
#          annotated_element.__code__.co_argcount != 0:
#             raise Exception("@_Target annotation may only be used on "\
#             "parameterless functions.")
#       if annotated_element.__doc__ is None or \
#          len(annotated_element.__doc__) == 0:
#          raise Exception("@_Target annotation requires inline documentation.")
#       name = annotated_element.__name__
#       while name[0] == '_':
#          name = name[1:]
#       if name in _Target.__all:
#          raise Exception("Another @_Target named " + name + \
#          " has already been defined.")
#       _Target.__all.update({name: self})
#       self.__function = annotated_element
#    
#    
#    ##
#    # The documentation of the underlying function.
#    @property
#    def doc(self) -> str:
#       return self.__function.__doc__
 
 
##
# The definition of a system architecture as defined by the BFD binutils.
class _Architecture:
    __all = {}
    
    
    ##
    # Default constructor.
    def __init__(self, name: str):
        self.__name = name
    
    
    ##
    # The architecture's name as defined by the BFD binutils.
    @property
    def name(self) -> str:
        return self.__name
    
    
    ##
    # Converts this into a string representation. This string representation
    # contains:
    # - the name
    def __str__(self) -> str:
        return self.__name
    
    
    ##
    # Returns the path to a binutil with a given name. Valid names are:
    # - as for the assembler
    # - ld for the linker
    # If the binutil has not been installed in the "tools" directory, the
    # required sources are downloaded and validated and the binutils are built.
    #
    # If the build fails, all logs are copyied into the "logs" directory.
    def get_binutil(self, name: str) -> str:
        from os.path import isfile, join
        assert(name in ["ld", "as"])
        path = join(_tool_dir, "bin", self.triplet + "-" + name)
        if not isfile(path):
            _build("https://ftp.gnu.org/gnu/binutils/binutils-2.26.tar.bz2",
                self.triplet)
        return path
    
    
    ##
    # Returns a triplet for Dr.Grätz.OS on the targeted architecture.
    @property
    def triplet(self) -> str:
        return self.__name + "-elf"
    
    
    ##
    # Returns an architecture by its name. If it has not yet been defined, it
    # is created.
    @staticmethod
    def get(name: str):
        if name not in _Architecture.__all:
            _Architecture.__all.update({name: _Architecture(name)})
        return _Architecture.__all[name]
 
 
##
# Representation of a possible target platform. The target platforms are
# determined by the linker scrips in the directory "src/kernel". Each linker
# script must be named "platform".ld. I.e. the linker script
# "src/kernel/i386.ld" corresponds to the platform i386.
#
# The linkerscript must define the target architecture using the OUTPUT_ARCH
# linker command.
class _Platform:
    __all = None
    __generic_includes = None
    __generic_header_files = None


    ##
    # Returns a dictionary of all supported platforms, indexed by their name.
    @staticmethod
    def get_all() -> _ReadOnlyDict:
        if _Platform.__all is None:
            result = {}
            for linker_script in _Directory.src.get_files([".ld"]):
                current = _Platform(linker_script)
                result.update({current.name: current})
            _Platform.__all = _ReadOnlyDict(result)
        return _Platform.__all
    
    
    ##
    # Constructor
    def __init__(self, path_to_init_script: str):
        from os.path import basename, join, splitext
        name, ext = splitext(basename(path_to_init_script))
        assert(ext == ".ld")
        self.__name = name
        self.__architecture = None
        self.__cpu = None
        self.__float_abi = None
        self.__format = "elf"
        self.__header_files = None
        self.__include_dirs = \
            _Platform.get_generic_includes() + \
            [_Directory.src.get(join("include", name))]
    
    
    ##
    # The CPU of this as it is used for the mtune parameter for gcc.
    @property
    def cpu(self) -> str:
        self.__parse_linker_script()
        return self.__cpu
    
    
    ##
    # The architecture, on which this is built on.
    @property
    def architecture(self) -> _Architecture:
        self.__parse_linker_script()
        return self.__architecture
    
    
    ##
    # Converts this into a string representation. The string representation
    # contains the name.
    def __str__(self) -> str:
        return self.__name
    
    
    ##
    # The float abi parameter for ARM architectures as it is used for the
    # mfloat-abi parameter for gcc.
    @property
    def float_abi(self) -> str:
        self.__parse_linker_script()
        return self.__float_abi
 
 
    ##
    # The name of this
    @property
    def name(self) -> str:
        return self.__name
    
    
    ##
    # Invoke the clang compiler to create a result file from a source file. If
    # the file extension of the result is ".bc", llvm bitcode is generated.
    # Compilation will be skipped, if the result file exists and is newer than
    # the source file or any header files in the include path and its
    # sub-directories.
    def compile(self, source_file: str, result: str) -> None:
        from os.path import dirname, splitext
        assert(isinstance(source_file, str))
        assert(isinstance(result, str))
        if not _needs_update(result,
            self.header_files +
            _Directory(dirname(source_file)).get_header_files()):
            return
        _Directory(dirname(result)).enforce()
        command_line = [
            "clang",
            "-c", source_file,
            "-nostdinc",
            "-o", result,
            "-fno-rtti",
            "-DVERBOSE",
        ]
        if result.endswith(".bc"):
            command_line += ["-emit-llvm"]
        else:
            command_line += [
                "--target=" + self.target,
                "-mtune=" + self.cpu
            ]
            if self.float_abi is not None:
                command_line += ["-mfloat-abi=" + self.float_abi]
        extension = splitext(source_file)[1]
        if extension == ".cpp":
            command_line += ["--std=c++11"]
        for include in self.includes + _Platform.get_generic_includes():
            command_line += ["-I", include.path]
        _invoke(command_line)
    
    
    ##
    # Returns a list of all platform specific include directories.
    @property
    def includes(self) -> _ReadOnlyList:
        from os.path import join
        return _ReadOnlyList([
            _Directory.src.get(join("include", self.name))
        ])
    
    ##
    # Parses the linker script to determine the values of the following
    # properties:
    # - architecture
    def __parse_linker_script(self) -> None:
        from logging import warning
        from os.path import join
        global _src_dir
        if self.__architecture is not None:
            return
        script_path = join(_Directory.src.path, "kernel", self.__name + ".ld")
        with open(script_path) as linker_script:
            for line in linker_script.read().splitlines():
                line = line.strip()
                if len(line) == 0:
                    continue
                if line.startswith("OUTPUT_ARCH(") and line.endswith(")"):
                    self.__architecture = _Architecture.get(line[12:-1])
                elif line.startswith("OUTPUT_FORMAT(") and line.endswith(")") and \
                    self.__architecture is None:
                    self.__architecture = "elf-" + _Architecture.get(line[14:-1])
                elif line.startswith("/*") and line.endswith("*/"):
                    line = line[2:-2].strip().split()
                    if len(line) < 3 or line[1] != "=":
                        continue
                    if line[0] == "CPU":
                       self.__cpu = line[2]
                    elif line[0] == "FLOAT-ABI":
                       self.__float_abi = line[2]
        if self.__cpu is None:
            warning("/* CPU = value */ not found in " + script_path +
                ", defaulting to \"generic\"")
            self.__cpu = "generic"
        if self.__architecture.name == "arm":
            if self.__float_abi is None or self.__float_abi not in \
                ["soft", "softfp", "hard"]:
                warning("/* FLOAT-ABI = value */ invalid for arm in " + \
                    script_path + ", defaulting to \"soft\"")
                self.__float_abi = "soft"
        else:
            if self.__float_abi is not None:
                warning("/* FLOAT-ABI = value */ has no effect for non arm in " + \
                    script_path)
        if self.__architecture is None:
            raise Exception(
                "definition of OUTPUT_ARCH not found in " + script_path)
    
    
    ##
    # Returns a list of include directories, which are used for
    # all platforms.
    @staticmethod
    def get_generic_includes() -> _ReadOnlyList:
        from os.path import join
        if _Platform.__generic_includes is None:
            _Platform.__generic_includes = _ReadOnlyList([
                _Directory.src.get("include"),
                _Directory.src.get(join("ext", "include"))
            ])
        return _Platform.__generic_includes
    
    
    ##
    # Returns a list of all header files, which lie in one of the
    # @ref get_generic_includes directories, but in no no directory
    # referenced by any @ref includes of any _Platform.
    @staticmethod
    def get_generic_header_files() -> _ReadOnlyList:
        if _Platform.__generic_header_files is None:
            result = []
            for include in _Platform.get_generic_includes():
                result += include.get_header_files()
            excludes = []
            for platform in _Platform.get_all().values():
                for include in platform.includes:
                    excludes += include.get_header_files()
            for exclude in excludes:
                result.remove(exclude)
            _Platform.__generic_header_files = _ReadOnlyList(result)
        return _Platform.__generic_header_files
    
    
    ##
    # Returnsa list of all header files required to compile this.
    @property
    def header_files(self) -> _ReadOnlyList:
        if self.__header_files is None:
            result = _Platform.get_generic_header_files()
            for include in self.includes:
                result += include.get_header_files()
            self.__header_files = result
        return self.__header_files
    
    
    ##
    # The target triplet.
    @property
    def target(self) -> str:
       self.__parse_linker_script()
       return self.__architecture.name + "-dr.grätz-" + self.__format
    
    
#    ##
#    # Compiles a source file for the target platform.
#    def compile(self, source_file: str) -> str:
#       from os.path import basename, dirname, join, relpath, splitext
#       assert(isinstance(source_file, str))
#       global _use_bitcode
#       name, platform_name = splitext(splitext(source_file)[0])
#       dir = relpath(dirname(source_file), "src")
#       if platform_name == "":
#          outfile = basename(name) + ".o"
#       else:
#          outfile = basename(name) + platform_name + ".o"
#       result = join(_obj_dir, self.name, dir, outfile)
#       if platform_name[1:] == self.name or platform_name == "":
#          self.__clang(source_file, result)
#       else:
#          return None
#       return result
 
 
##
# Checks, wether a specific result file has to be updated. This is the case,
# if one of the following conditions is met:
# - the result file does not exist
# - a rebuilt has been enforced
# - the file is older than any of the input files (or times)
def _needs_update(result: str, input_files_and_times: list) -> bool:
   def __get_latest(files_and_times: list) -> float:
      from os.path import getmtime, isfile
      assert(isinstance(files_and_times, list))
      result = 0.0
      for file_or_time in files_and_times:
         if isinstance(file_or_time, float):
            time = file_or_time
         elif isinstance(file_or_time, str):
            if isfile(file_or_time):
               time = getmtime(file_or_time)
            else:
               time = 0.0
         elif file_or_time is not None:
            raise Exception("Unsupported type " + type(file_or_time).__name__)
         result = max(time, result)
      return result
 
 
   assert(isinstance(input_files_and_times, (list, _ReadOnlyList)))
   from os.path import exists, getmtime
   #if invokation.rebuild:
   #   return True
   #if not invokation.incremental:
   #   return False
   if not exists(result):
      return True
   return getmtime(result) < __get_latest(input_files_and_times)
 
 
##
# Invokes a command line (a list of arguments, where the argument indexed with
# zero is the command).
#
# The output and the error output is redirected to the logger with log levels
# DEBUG and ERROR respectively.
def _invoke(command_line: list, working_dir: str=".") -> None:
   from logging import debug, DEBUG, ERROR, getLogger
   from os import environ
   from subprocess import check_call
   from threading import Thread
    
    
   ##
   # A thread, that logs all messages to stdout 
   class __ProcessLogger(Thread):
      def __init__(self, log_level: int):
         assert(isinstance(log_level, int))
         Thread.__init__(self)
         from os import pipe
         self.__in, self.__out = pipe()
         self.__log_level = log_level
         self.start()
  
  
      def run(self) -> None:
         from os import read
         line = ""
         last_char = 0
         while True:
            try:
               data = read(self.__in, 1024)
            except:
               break
            data = data.decode("utf-8")
            for char in data:
               line_break = False
               if char == "\r":
                  line_break = True
               elif char == "\n":
                  line_break = last_char != "\r"
               else:
                  line += char
               last_char = char
               if line_break:
                  getLogger().log(self.__log_level, line)
                  line = ""
         if line != "":
            getLogger().log(self.__log_level, line)
       
       
      @property
      def out(self):
         return self.__out
 
          
      def close(self) -> None:
         from os import close
         if self.__out is not None:
            close(self.__out)
            self.__out = None
         if self.__in is not None:
            close(self.__in)
            self.__in = None
 
             
      def __enter__(self):
         return self
       
       
      def __exit__(self, exc_type, exc_value, traceback) -> None:
         self.close()
    
    
   assert(isinstance(command_line, list))
   for element in command_line:
      assert(isinstance(element, str))
   assert(isinstance(working_dir, str))
   # stdin, stdout, stderr
   environment = {
      "PATH": environ["PATH"]
   }
   command_line_str = ">"
   for element in command_line:
      command_line_str += " " + element
   debug(command_line_str)
   with __ProcessLogger(DEBUG) as out_logger:
      with __ProcessLogger(ERROR) as err_logger:
         check_call(command_line, \
            cwd=working_dir, \
            env=environment, \
            stdout = out_logger.out, \
            stderr = err_logger.out)
 
 
##
# Initializes the logging for the build script. The contents of the directory
# "logs" are erased and a new log file concated of the program name of this
# script and the extension ".log" is created. The log level for this log is set
# to DEBUG. Another log handler is assigned to the console output with the log
# level INFO. It trims log messages to a length of 79 characters. Longer
# messages are succeeded by the character sequence "...". All messages are
# colored depending on the log level (INFO: blue, WARNING: yellow, ERROR: red,
# other: gray)
def __init_logging() -> None:
    from logging import DEBUG, ERROR, Formatter, getLogger, INFO, \
        LogRecord, WARNING, StreamHandler
    from os.path import abspath, basename, dirname, join, splitext
    from sys import argv
    
    
    ##
    # Formatter for the console output. Prints  the different log levels in
    # different colors (ERROR = red, WARNING = yellow, INFO = blue,
    # other = white) and trims the lines to the screen width.
    class __PrettyFormatter(Formatter):
        def __init__(self):
            Formatter.__init__(self)
       
        def format(self, record: LogRecord) -> str:
            from os import linesep
            global _no_color, _max_line_width
            assert(isinstance(record, LogRecord))
            # name, levelno, levelname, pathname, lineno, msg, args, exc_info,
            # func, sinfo
            level = record.levelno
            if level >= ERROR:
                bold = True
                color = 1
            elif level >= WARNING:
                bold = True
                color = 3
            elif level >= INFO:
                bold = True
                color = 4
            else:
                bold = False
                color = 7
            if _CommandLine.use_color:
                result = "\033[" + str(30 + color)
                if bold:
                    result += ";1"
                result += "m"
            else:
                result = ""
            n = 1
            for line in record.msg.splitlines(True):
                if n == 10:
                    result += "  ..."
                    break
                n += 1
                if len(line) > _CommandLine.max_line_width:
                    result += line[0:_CommandLine.max_line_width - 3] + "..." + linesep
                else:
                    result += line
            if _CommandLine.use_color:
                result += "\033[0m"
            return result
 
 
    
    ##
    # Custom exception handler. Logs the exception type plus and the exception
    # parameters followed by a stack trace.
    def __exception_handler(exc_type: type, value: BaseException, traceback) \
        -> None:
        from logging import fatal
        from os import linesep
        message = exc_type.__name__
        first = True
        for arg in value.args:
            if first:
                message += ": "
                first = False
            else:
                message += ", "
            message += str(arg)
        tb = traceback
        stack = []
        while tb is not None:
            stack.append(tb)
            tb = tb.tb_next
        for element in reversed(stack):
            frame = element.tb_frame
            message += linesep + "  " + frame.f_code.co_filename + \
                ", Zeile " + str(frame.f_lineno)
        fatal(message)
 
 
    console = StreamHandler()
    console.setFormatter(__PrettyFormatter())
    console.setLevel(INFO)
    logger = getLogger()
    logger.setLevel(DEBUG)
    logger.addHandler(console)
    import sys
    sys.excepthook = __exception_handler
 
 
##
# The command line with its parameters
class _CommandLine:
    max_line_width = 79
    use_color = True
    rebuild = False
    
    @staticmethod
    def evaluate() -> None:
        from sys import argv
        i = 1
        while i < len(argv):
            current = argv[i]
            i += 1
            try:
                if current == "--help" or current == "-?":
                    _CommandLine.__print_help()
                elif current == "--no-color":
                    _CommandLine.use_color = False
                elif current == "--max-line-width":
                    _CommandLine.max_line_width = int(argv[i])
                    i += 1
                elif current == "--rebuild":
                    _CommandLine.rebuild = True
                else:
                    raise ValueError("Invalid command line option.")
            except Exception as e:
                print("Failed to evaluate command line parameter: " + current)
                print(e.args[0])
                _CommandLine.__print_help()


    ##
    # Prints an online help to stdout.
    @staticmethod
    def __print_help() -> None:
        from sys import argv
        print("Usage: " + argv[0] + " [options]")
        #print()
        #print("Valid targets are (default = all):")
#         names = list(_Target.all.keys())
#         names.sort()
#         for name in names:
#             descr = _Target.all[name].doc
#             if descr is None:
#                 print(name)
#             else:
#                 print(name + "\t" + descr)
        print()
        print("Valid options are:")
        print("--help, -?            print this help and exit")
        print("--no-color            don't use ANSI colors for output")
        print("--max-line-width x    print a maximum of x characters per line (default value: 79)")
        print("--rebuild             rebuild all binaries rather than an incremental build")
        exit()
     
     
# ##
# # Creates the doxygen documentation. Logs a warning, if doxygen is not
# # installed. The file "src/doxygen.config" is used as doxygen configuration.
# @_Target
# def __doc() -> None:
#    """Create the doxgen documentation."""
#    from logging import info, warning
#    info("creating documentation")
#    _erase(_doc_dir, [_doc_dir])
#    try:
#       _invoke(["doxygen", "src/doxygen.config"])
#    except FileNotFoundError as e:
#       warning("doxygen not installed - no documentation generated")
# 
# 
# ##
# # Compiles all source files in the "src" folder, if they have been changed.
# # Object files, which have to corresponding source file, are deleted.
# #
# # All files with the extensions ".c", ".cpp", and ".S" within the "src"
# # folder and all its subfolders are considered source files and compiled
# # using the clang compiler. An platform file extension can preceed the
# # file extension, such as ".rasppi", or ".i386". These files are only
# # compiled for the target platform.
# #
# # The output file is stored in an platform dependent subfolder within
# # the "obj" folder. The relative path of the output file to this
# # platform dependent subfolder is identical to the relative path of
# # the source filde to the "src" folder. The file extension of the output
# # file is ".o".
# #
# # Examples:
# # input file                  | output files
# # ----------------------------|---------------------------------
# # src/kernel/boot.i386.s      | obj/i386/kernel/boot.o
# # src/kernel/main.s           | obj/*/kernel/main.o
# @_Target
# def __compile() -> None:
#    """Build all binaries."""
#    from logging import info
#    global _src_dir
#    result = [_obj_dir]
#    _enforce_dir(_obj_dir)
#    for platform in _Platform.get_all().values():
#       info("compiling for " + platform.name)
#       for source_file in __get_files(_src_dir, [".c", ".cpp", ".S"]):
#          output = platform.compile(source_file)
#          if output is not None:
#             result.append(output)
#    _erase(_obj_dir, result)
# 
# 
# ##
# # Link all object files, if they have been changed. For each subfolder in the
# # architecture dependent "obj" folder, an artifact is created. The result ist
# # stored in a architecture dependent subfolder within the "bin" folder. If the
# # "src" folder contains a architecture dependent linker script for the
# # artifact, it is used.
# #
# # Examples:
# #
# # input files         | output file       | linker script
# # --------------------|-------------------+-------------------
# # obj/i386/kernel/*.o | bin/i386/kernel   | src/kernel/i386.ld
# # obj/rasppi/libc/*.o | bin/rasppi/libc.a | src/libc/rasppi.ld
# #
# # The required binutils are downloaded to "tools/src" and installed to
# # "tools/bin".
# #
# @_Target
# def __link() -> None:
#    """Compile & link all changed files."""
#    from logging import info
#    from os import listdir
#    from os.path import dirname, isfile, join
#    __compile()
#    result = [_bin_dir]
#    _enforce_dir(_bin_dir)
#    for platform_name in listdir(_obj_dir):
#       platform = _Platform.get_all()[platform_name]
#       message = False
#       linker = platform.architecture.get_binutil("ld")
#       for executable in listdir(join(_obj_dir, platform.name)):
#          if not message:
#             info("linking for " + platform.name)
#             message = True
#          input_files = __get_files(join(_obj_dir, platform.name, executable))
#          output_file = join(_bin_dir, platform.name, executable)
#          script = join(_src_dir, executable, platform.name + ".ld")
#          result.append(output_file)
#          result.append(output_file + ".map")
#          if not _needs_update(output_file, input_files + [script]):
#             continue
#          _enforce_dir(dirname(output_file))
#          command_line = [
#             linker,
#             "-o", output_file,
#             "-M=" + output_file + ".map",
#             "--nostdlib",
#             "--strip-all"
#          ] + input_files
#          if isfile(script):
#             command_line += ["-T", script]
#          _invoke(command_line)
#    _erase(_bin_dir, result)
# 
# 
# ##
# # Test the builds.
# @_Target
# def __test() -> None:
#    """Test the builds."""
#    from logging import info
#    from os import listdir
#    from os.path import join
#    __link()
#    for platform_name in listdir(_bin_dir):
#       info("testing " + platform_name)
#       platform = _Platform.get_all()[platform_name]
#       command_line = [
#          "qemu-system-" + platform.architecture.name,
#          "-kernel", join(_bin_dir, platform.name, "kernel"),
#          "-serial", "file:" + join(_logs_dir, "test-" + platform.name + ".log"),
#          "-initrd", join(_bin_dir, platform.name, "launcher")
#       ]
#       _invoke(command_line)
# 
# 
# ##
# # Complete build.
# @_Target
# def __all() -> None:
#    """Update and test all contents."""
#    __test()
#    __doc()
# 
# 
# ##
# # Delete all artefacts.
# @_Target
# def __clean() -> None:
#    """Delete all artifacts."""
#    _enforce_dir(_obj_dir, True)
#    _enforce_dir(_bin_dir, True)
#    _enforce_dir(_doc_dir, True)
# 
# 
# ##
# # Complete rebuild.
# @_Target
# def __rebuild() -> None:
#    """Clear stale data and create all contents."""
#    __clean()
#    __all()
# 
# 
# ##
# # Builds a thirs party tool. This includes the following steps:
# # - downloading and verifying the sources
# # - unpacking the sources
# # - configuring the bundle ("configure")
# # - making the bundle ("make all")
# # - installing the bundle ("make install")
# def _build(source_url: str, target_triplet: str) -> None:
#    ##
#    # Downloads a source package from a given url to the tools/src directory.
#    # Returns the path to the downloaded package. Alsow downloads the packages
#    # signature and the corresponding key ring and verifies the package.
#    def _download(url: str) -> str:
#       from logging import info
#       from os.path import basename, dirname, expanduser, isfile, join
#       from urllib.request import urlopen
#       assert(isinstance(url, str))
#       target_dir = join(_tool_dir, "src")
#       _enforce_dir(target_dir)
#       if url.startswith("https://ftp.gnu.org/"):
#          signature = url + ".sig"
#          keyring = "https://ftp.gnu.org/gnu/gnu-keyring.gpg"
#       elif url.startswith("https://www.uclibc.org/"):
#          signature = url + ".sign"
#          keyring = None
#       else:
#          raise NotImplementedError("Unsupported provider for URL " + url)
#       for current in [url, signature, keyring]:
#          if current is None:
#             continue
#          with _TempFile(join(target_dir, basename(current))) as target:
#             if target.exists:
#                continue
#             info("downloading " + current)
#             source = urlopen(current)
#             while True:
#                data = source.read(1024)
#                if len(data) == 0:
#                   break
#                target.write(data)
#       if signature is not None:
#          if keyring is not None:
#             keyring = ["--keyring", join(target_dir, basename(keyring))]
#          else:
#             keyring = []
#          info("verifying " + basename(url))
#          _invoke(["gpg"] + keyring +
#             ["--verify", join(target_dir, basename(signature))])
#       return join(target_dir, basename(url))
# 
# 
#    ##
#    # Unpacks an archive into a directory. The unpacked files are located in a
#    # subdirectory of the target_dir, which is named like the archive without
#    # its file extensions. If the archive contains a single directory, all its
#    # contents are moved into the newly created subdirectory.
#    def _unpack(archive: str, target_dir: str) -> str:
#       from logging import info, warning
#       from os import listdir, rename, rmdir
#       from os.path import  basename, getsize, isabs, isdir, isfile, join, normpath
#       from tarfile import open
#       from tempfile import mkdtemp
#       assert(isinstance(archive, str))
#       assert(isinstance(target_dir, str))
#       info("unpacking " + basename(archive))
#       tar = open(archive)
#       while True:
#         file = tar.next()
#         if file is None:
#            break
#         path = normpath(file.name)
#         if isabs(path) or path.startswith("..") or path.startswith("/"):
#            warning("skipping file " + path + " as its path is considered unsafe")
#            continue
#         if file.isfile():
#            if not isfile(path) or file.mtime > getmtime(path) or \
#               getsize(path) != file.size:
#               tar.extract(file, target_dir)
#         else:
#            raise Exception("Unsupported file type for " + file.name + " in " +
#               archive)
#       contents = listdir(target_dir)
#       if len(contents) == 1:
#          root = join(target_dir, contents[0])
#          if isdir(root):
#             temp = mkdtemp()
#             rmdir(temp)
#             try:
#                rename(root, temp)
#                rmdir(target_dir)
#                rename(temp, target_dir)
#             except:
#                _erase(temp)
#                raise
#       return target_dir
#    
#    
#    from logging import info
#    from multiprocessing import cpu_count
#    from os import rename, walk
#    from os.path import abspath, basename, dirname, join, relpath
#    from tempfile import mkdtemp
#    archive = _download(source_url)
#    source = mkdtemp()
#    build = mkdtemp()
#    package = basename(archive)
#    try:
#       _unpack(archive, source)
#       info("configuring " + package + " for " + target_triplet)
#       _invoke([
#          join(relpath(source, build), "configure"),
#          "--quiet",
#          "--prefix=" + abspath(_tool_dir),
#          "--with-sysroot=" + abspath(join(_tool_dir, "lib")),
#          "--with-lib-path=" + abspath(join(_tool_dir, "lib")),
#          "--target=" + target_triplet,
#          "--disable-nls",
#          "--disable-werror"], build)
#       info("making " + package + " for " + target_triplet)
#       _invoke(["make", "all", "--jobs=" + str(cpu_count())], build)
#       info("installing " + package + " for " + target_triplet)
#       _invoke(["make", "install"], build)
#    except:
#       info("saving log files to logs/*")
#       for root, dirs, files in walk(build):
#          for file in files:
#             if file.endswith(".log"):
#                src = join(root, file)
#                dst = join(_logs_dir, relpath(src, build))
#                _enforce_dir(dirname(dst))
#                rename(src, dst)
#    finally:
#       _erase(source)
#       _erase(build)
#    return


##
# Compile all source code files. Source code files are located within
# the "src" folder and have one of the following extensions:
# - .S for assembler files processed by the c preprocessor
# - .cpp for C++ files
def __compile() -> None:
    from logging import info
    from os.path import join, relpath, splitext
    for platform in _Platform.get_all().values():
        info("compiling for " + platform.name)
        for source_file in _Directory.src.get_files([".S", ".cpp"]):
            rel_path, ext = splitext(relpath(source_file, _Directory.src.path))
            output_file = join(_Directory.obj.path, rel_path) + ".o"
            rel_path, target_platform = splitext(rel_path)
            if target_platform == "" or target_platform == platform.name:
                platform.compile(source_file, output_file)


_CommandLine.evaluate()
__init_logging()
_Directory.__static_init__()
__compile()
