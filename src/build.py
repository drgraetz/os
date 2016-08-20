#!/bin/python3
 
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
    # Returns a list of all sub-directories.
    def list_dirs(self):
        from os.path import isdir, join
        return [_Directory(candidate)
            for candidate in self.list_contents()
            if isdir(candidate)]
        return result
    
    
    ##
    # Returns a list of all contained file elements (directories, files and links)
    def list_contents(self):
        from os import listdir
        from os.path import join
        result = [join(self.__path, candidate)
            for candidate in listdir(self.__path)]
        return result
        
    
    
    ##
    # Returns the path of this.
    def __str__(self) -> str:
        return self.__path
     
     
    ##
    # Returns a sub-directory.
    def get(self, rel_path: str):
        from os.path import join
        path = join(self.__path, rel_path)
        return _Directory(path)
    
    
    ##
    # Appends the path of this
    def join(self, *args):
        from os.path import join
        return join(self.__path, *args)
     
     
    ##
    # Static initializer. Must be called prior to any other usage of
    # _Directory.
    @staticmethod
    def __static_init__():
        from logging import info
        from os.path import dirname
        from sys import argv
        _Directory.src = _Directory(dirname(argv[0]))
        _Directory.base = _Directory.src.parent
        _Directory.logs = _Directory.base.enforce("logs", True)
        _Directory.logs.__create_log_file()
        info("preparing directories")
        _Directory.obj = _Directory.base.enforce("obj", _CommandLine.rebuild)
        _Directory.doc = _Directory.base.enforce("doc", True)
        _Directory.bin = _Directory.base.enforce("bin", _CommandLine.rebuild)
        _Directory.tool = _Directory.base.enforce("tools")
     
     
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
    # Checks, whether the provided path is either relative to this or absolute
    # and resides in this.
    def within(self, path: str) -> bool:
        from os.path import isabs
        return not isabs(path) or not self.rel_path(path).startswith("..")
    
    
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
    # Returns the name of this.
    @property
    def name(self) -> str:
        from os.path import basename
        return basename(self.__path)
     
     
    ##
    # Erases all contents of this except for a list of files which shall not be
    # deleted. The erase machanisms distingushes between symbol links (which are
    # unlinked), directories (which are recursively deleted), and files (which are
    # removed). Raises an exception, if a file, which is not explicitly excluded,
    # cannot be erased.
    def erase_contents(self, keep_files: list=[], erase_me: bool=False) -> None:
        from os import listdir, remove, rmdir, unlink, walk
        from os.path import isdir, islink, join
        for file in reversed(self.get_files(None, True)):
            if file in keep_files:
                pass
            elif islink(file):
                unlink(file)
            elif isdir(file):
                if len(listdir(file)) == 0:
                    rmdir(file)
            else:
                remove(file)
        if erase_me and len(keep_files) == 0:
            rmdir(self.__path)
    
    
    ##
    # Returns an absolute path relative to this.
    def rel_path(self, path: str) -> str:
        from os.path import relpath
        assert(isinstance(path, (str, _Directory)))
        if isinstance(path, _Directory):
            path = path.path
        return relpath(path, self.__path)
      
      
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
    
    ## Returns a string representation of this.
    def __str__(self) -> str:
        return self.__base.__str__()
 
 
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
    
    
    def __str__(self) -> str:
        return self.__base.__str__()
     
     
##
# A temporary file or directory. It is typically used within a with
# statement. It will clean itself up in case of an exception and will
# prevail on a given target path on normal termination.
class _TempFile:
     
     
    def __init__(self, path: str, dir: bool = False):
        from os.path import dirname, isdir, isfile
        from tempfile import mkdtemp, mkstemp
        assert(isinstance(path, str))
        assert(isinstance(dir, bool))
        self.__path = path
        _Directory(dirname(path)).enforce()
        self.__tmp_handle = None
        self.__tmp_path = None
        if dir:
            self.__tmp_path = mkdtemp()
        else:
            if not isfile(self.__path):
                self.__tmp_handle, self.__tmp_path = mkstemp()

     
    @property
    def exists(self) -> bool:
        from os.path import exists
        return exists(self.__path)

     
    @property
    def path(self) -> str:
        return self.__path

     
    def __close(self) -> None:
        from os import close
        if self.__tmp_handle is not None:
            close(self.__tmp_handle)
            self.__tmp_handle = None
     

    def close(self) -> None:
        from os import rename
        self.__close()
        if self.__tmp_path is not None:
            rename(self.__tmp_path, self.__path)
            self.__tmp_path = None

     
    def cancel(self) -> None:
        from os import remove, rmdir
        from os.path import isdir, isfile
        self.__close()
        if isfile(self.__path):
            remove(self.__path)
        elif isdir(self.__path) is None:
            _Directory(self.__path).erase_contents()
            rmdir(self.__path)
        self.__tmp_path = None
        self.__tmp_handle = None
   
     
    def write(self, data) -> None:
        from os import write
        write(self.__tmp_handle, data)
   
   
    @property
    def tmp_path(self):
         return self.__tmp_path
   
     
    def __del__(self) -> None:
        self.close()
   
     
    def __enter__(self):
        return self
   
     
    def __exit__(self, exc_type, exc_value, traceback) -> None:
        if exc_value is None:
            self.close()
        else:
            self.cancel()
  
  
##
# Checks, wether a specific result file has to be updated. This is the case,
# if one of the following conditions is met:
# - the result file does not exist
# - a rebuilt has been enforced
# - the file is older than any of the input files (or times)
def _needs_update(result: str, input_files_and_times: list) -> bool:
   def __get_latest(files_and_times: list) -> float:
      from os.path import getmtime, isfile
      assert(isinstance(files_and_times, (list, _ReadOnlyList)))
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
def _invoke(command_line: list, working_dir: _Directory=None, env: dict = {}) -> None:
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
                try:
                    data = data.decode("utf-8")
                except:
                    data = [chr(e) for e in data]
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
     
    if working_dir is None:
        working_dir = _Directory.base
    assert(isinstance(command_line, list))
    for element in command_line:
        assert(isinstance(element, str))
    assert(isinstance(working_dir, _Directory))
    assert(isinstance(env, (dict, _ReadOnlyDict)))
    # stdin, stdout, stderr
    environment = dict(env)
    environment.update({
        "PATH": environ["PATH"],
        "LANG": "en_EN.UTF-8"
    })
    command_line_str = ">"
    for element in command_line:
        command_line_str += " " + element
    debug(environment)
    debug(command_line_str.strip())
    with __ProcessLogger(DEBUG) as out_logger:
        with __ProcessLogger(ERROR) as err_logger:
            check_call(command_line, \
                cwd=working_dir.path, \
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
                if n == 20:
                    result += "  ..."
                    break
                n += 1
                if len(line) > _CommandLine.max_line_width:
                    result += line[0:_CommandLine.max_line_width - 3] + "..."
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
        print()
        print("Valid options are:")
        print("--help, -?            print this help and exit")
        print("--no-color            don't use ANSI colors for output")
        print("--max-line-width x    print a maximum of x characters per line (default value: 79)")
        print("--rebuild             rebuild all binaries rather than an incremental build")
        exit()
      
      
# # ##
# # # Creates the doxygen documentation. Logs a warning, if doxygen is not
# # # installed. The file "src/doxygen.config" is used as doxygen configuration.
# # @_Target
# # def __doc() -> None:
# #    """Create the doxgen documentation."""
# #    from logging import info, warning
# #    info("creating documentation")
# #    _erase(_doc_dir, [_doc_dir])
# #    try:
# #       _invoke(["doxygen", "src/doxygen.config"])
# #    except FileNotFoundError as e:
# #       warning("doxygen not installed - no documentation generated")
# # 
# # 
  
  
##
# The representation of the contents of the build_infox.xml file.
class _BuildInfo:
    __doc = None
    __platforms = {}
    __signatures = []
    
    
    ##
    # The definition of a symbol passed to the linker.
    class LinkerSymbol:
        
        
        def __init__(self, xml):
            self.__name = xml.get('name')
            self.__value = xml.get('value')
        
        
        @property
        def name(self) -> str:
            return self.__name
        
        
        @property
        def value(self) -> str:
            return self.__value
    
    
    ##
    # A target platform.
    class Platform:
        
        
        def __init__(self, xml):
            from os.path import join
            self.__name = xml.get('name')
            self.__target_triplet = xml.get('target-triplet')
            self.__compiler_params = _ReadOnlyList(xml.get('compiler-params').split())
            self.__qemu_params = _ReadOnlyList(xml.get('qemu-params').split())
            self.__qemu = xml.get('qemu')
            self.__includes = {}
            self.__include_dirs = _ReadOnlyList([
                 _Directory.src.get("include"),
                 _Directory.src.get(join("ext", "include")),
                 _Directory.src.get(join("include", self.name))
            ])
            linker_symbols = []
            _BuildInfo._create("linker-symbol", _BuildInfo.LinkerSymbol, linker_symbols, xml)
            self.__linker_symbols = _ReadOnlyList(linker_symbols)
        
        
        ##
        # The linker-symbols defined in buildinfo.xml.
        @property
        def linker_symbols(self) -> _ReadOnlyList:
            return self.__linker_symbols
        
        
        ##
        # The platform's target triplet as defined in the target-triplet
        # attribute.
        @property
        def target_triplet(self) -> str:
            return self.__target_triplet
        
        
        ##
        # The platform's architecture (i.e. the first part of the target triplet).
        @property
        def architecture(self) -> str:
            return self.__target_triplet[:self.__target_triplet.index('-')]
        
        
        ##
        # The parameters passed to qemu.
        @property
        def qemu_params(self) -> list:
            return list(self.__qemu_params)
        
        
        ##
        # The parameters passed to the compiler.
        @property
        def compiler_params(self) -> list:
            return list(self.__compiler_params)
        
        
        ##
        # The name of the qemu executable.
        @property
        def qemu(self) -> str:
            return self.__qemu
        
        
        ##
        # The platform's name as defined in the name attribute.
        @property
        def name(self) -> str:
            return self.__name
    
        ##
        # Reads the include definitions of a source file and returns
        # a list of all included files.
        def read_includes(self, source_file: str) -> list:
            from logging import warning
            from os.path import abspath, dirname, isfile, join
            if source_file in self.__includes:
                return self.__includes[source_file]
            result = []
            line_no = 1
            for line in open(source_file).readlines():
                line = line.strip()
                if not line.startswith("#include"):
                    continue
                include = line[8:].strip()
                header = None
                if include.startswith('"') and include.endswith('"'):
                    candidate = join(dirname(source_file), include[1:-1])
                    if isfile(candidate):
                        header = candidate
                elif include.startswith('<') and include.endswith('>'):
                    name = include[1:-1]
                    for include_path in self.include_dirs:
                        candidate = include_path.join(name)
                        if isfile(candidate):
                            header = candidate
                            break
                if header is None:
                    warning("failed to resolve #include " + include +
                        " in " + source_file + ", line " + str(line_no))
                    continue
                header = abspath(header)
                if header in result:
                    continue
                result.append(header)
                line_no += 1
            file = 0
            while file < len(result):
                for include in self.read_includes(result[file]):
                    if include not in result:
                        result.append(include)
                file += 1
            self.__includes.update({source_file: result})
            return result
    
    
        ##
        # Returns a list of directories, that are included by the compiler.
        @property
        def include_dirs(self) -> _ReadOnlyList:
            return self.__include_dirs
        
        
        ##
        # Returns the path to the linker for this architecture. If the linker is
        # not installed, its sources are downloaded and it is built.
        #
        # If the build fails, all logs are copied into the "logs" directory.
        def get_linker(self) -> str:
            return _BuildInfo.Tool.get("ld", self.__target_triplet)
        
        
        ##
        # Returns the path to the compiler for this architecture. If the compiler
        # is not installed, its sources are downloaded and it is built.
        #
        # If the build fails, all logs are copied into the "logs" directory.
        def get_compiler(self) -> str:
            return _BuildInfo.Tool.get("clang")
    
    
    ##
    # A signature definition.
    class Signature:
        
        
        def __init__(self, xml):
            self.__uri = xml.get("uri")
            self.__public_key = xml.get("public-key")
            self.__key_ring = xml.get("key-ring")
            self.__extension = xml.get("extension")
        
        
        @property
        def uri(self) -> str:
            return self.__uri
        
        
        @property
        def public_key(self) -> str:
            return self.__public_key
        
        
        @property
        def key_ring(self) -> str:
            return self.__key_ring
        
        
        @property
        def extension(self) -> str:
            return self.__extension
    
    
    ##
    # A packet definition.
    class Package:
        __src_dir = None
        __unpack_list = None
        
        
        def __init__(self, xml):
            self.__url = xml.get("url")
            self.__dir = xml.get("dir")
            if self.__dir is None:
                self.__dir = "."
            self.__sub_packages = []
            _BuildInfo._create("sub-package", _BuildInfo.Package, self.__sub_packages, xml)
    
        
        @staticmethod
        def get_src_dir() -> _Directory:
            if _BuildInfo.Package.__src_dir is None:
                _BuildInfo.Package.__src_dir = _Directory.tool.get("src")
            return _BuildInfo.Package.__src_dir
        
        
        @staticmethod
        def get_unpack_list() -> str:
            if _BuildInfo.Package.__unpack_list is None:
                _BuildInfo.Package.__unpack_list = _Directory.tool.join(".unpack_list")
            return _BuildInfo.Package.__unpack_list
        
        
        @staticmethod
        def __untar(archive: str, output_dir: _Directory) -> None:
            from logging import info, warning
            from os import rename, sep, symlink
            from os.path import basename, dirname, exists, isabs, isdir, normpath
            from tarfile import open
            info("unpacking " + basename(archive))
            tar = open(archive)
            files = []
            while True:
                file = tar.next()
                if file is None:
                    break
                path = normpath(file.name)
                if isabs(path) or path.startswith("..") or path.startswith("/"):
                    warning("skipping file " + path + " as its path is considered unsafe")
                    continue
                files.append(file.name)
                if exists(output_dir.join(file.name)):
                    continue
                tar.extract(file, output_dir.path)
            roots = _create_tree_from_paths(files)
            if len(roots) == 1:
                root = output_dir.get(roots.keys()[0])
                for file in root.list_contents():
                    rename(file, output_dir.join(root.rel_path(file)))
                root.erase_contents([], True)
                symlink(".", root.path)
    
    
        ##
        # Unpacks this archive into a directory. The unpacked files are located in
        # "tools/build/src" and and appended sub-directory, if defined so in
        # build_info.xml
        #
        # @return the directory with the extracted contents
        def unpack(self) -> _Directory:
            from os import linesep
            from os.path import isfile, join
            output_dir = _BuildInfo.Package.get_src_dir().get(self.__dir)
            unpack_list = _BuildInfo.Package.get_unpack_list()
            if isfile(unpack_list):
                packages = open(unpack_list).readlines()
                unpacked = self.__url in packages or self.__url + linesep in packages
            else:
                unpacked = False
            if not unpacked:
                archive = _BuildInfo._download(self.__url)
                _BuildInfo._verify(self.__url, archive)
                _BuildInfo.Package.__untar(archive, output_dir)
                file = open(unpack_list, "a")
                file.write(self.__url + linesep)
                file.close()
            for sub_package in self.__sub_packages:
                sub_package.unpack()
            return output_dir
    
    
    ##
    # A third party tool.
    class Tool:
        __build_dir = None
        __executables = {}
        
        
        @staticmethod
        def get_build_dir() -> _Directory:
            if _BuildInfo.Tool.__build_dir is None:
                _BuildInfo.Tool.__build_dir = _Directory.tool.enforce("build")
            return _BuildInfo.Tool.__build_dir
        
        
        @staticmethod
        def __createTargetString(target_triplet: str) -> str:
            if target_triplet is None:
                return ""
            else:
                return " for " + target_triplet
        
        
        def __init__(self, xml):
            self.__name = xml.get("name")
            self.__package = _BuildInfo.Package(xml.find("package"))
            env = {}
            for element in xml.findall("env"):
                env.update({element.get("variable"): element.get("value")})
            executables_nodes = xml.findall("executable")
            if len(executables_nodes) == 0:
                executables = [self.__name]
            else:
                executables = []
                for element in executables_nodes:
                    executables.append(element.get("name"))
            for executable in executables:
                if executable in _BuildInfo.Tool.__executables:
                    raise Exception("Ambigious definition of executable '" + executable + "' in buildinfo.xml")
                _BuildInfo.Tool.__executables.update({
                    executable: self
                })
            if self.__name != "clang":
                bin_dir = _Directory.tool.get("bin")
                env.update({
                    "CC": bin_dir.join("clang"),
                    "CXX": bin_dir.join("clang++")
                })
            self.__env = _ReadOnlyDict(env)
        
        
        def __configure(self, source_dir: _Directory, target_triplet: str):
            from logging import info
            from os.path import isfile
            info("configuring " + self.__name +
                _BuildInfo.Tool.__createTargetString(target_triplet))
            configure = source_dir.join("configure")
            cmake_list = source_dir.join("CMakeLists.txt")
            build_dir = _BuildInfo.Tool.get_build_dir()
            build_dir.erase_contents()
            if isfile(cmake_list):
                command_line = [
                    "cmake",
                    "-Wno-dev",
                    build_dir.rel_path(source_dir.path),
                    "-G", "Unix Makefiles",
                    "-DCMAKE_INSTALL_PREFIX=" + _Directory.tool.path]
            elif isfile(configure):
                lib_path = _Directory.tool.rel_path("lib")
                command_line = [
                    build_dir.rel_path(configure),
                    "--quiet",
                    "--prefix=" + _Directory.tool.path,
                    "--with-sysroot=" + lib_path,
                    "--with-lib-path=" + lib_path,
                    "--disable-nls",
                    "--disable-werror"]
            else:
                raise Exception("Unsupported build chain for " + self.name)
            if target_triplet is not None:
                command_line += ["--target=" + target_triplet]
            self.__invoke(command_line)
        
        
        @property
        def name(self) -> str:
            return self.__name
    
    
        def __make(self, target_triplet: str) -> None:
            from logging import info
            from multiprocessing import cpu_count
            info("making " + self.name +
                _BuildInfo.Tool.__createTargetString(target_triplet))
            self.__invoke(["make", "all", "--jobs=" + str(cpu_count())])
        
        
        def __install(self, target_triplet: str) -> None:
            from logging import info
            info("installing " + self.name +
                _BuildInfo.Tool.__createTargetString(target_triplet))
            self.__invoke(["make", "install"])
        
        
        def __invoke(self, command_line: list) -> None:
            assert(isinstance(command_line, list))
            _invoke(command_line, _BuildInfo.Tool.get_build_dir(), self.__env)


        ##
        # Builds this. The required sources are downloaded and verified.
        def build(self, target_triplet:str) -> None:
            from os import remove
            source_dir = self.__package.unpack()
            _BuildInfo.Tool.get_build_dir().erase_contents()
            try:
                self.__configure(source_dir, target_triplet)
                self.__make(target_triplet)
                self.__install(target_triplet)
            except:
                _BuildInfo.Tool.__save_logs()
                raise
            remove(_BuildInfo.Package.get_unpack_list())
            _BuildInfo.Package.get_src_dir().erase_contents()
            
    
        ##
        # Returns the path to a third-party tool. If the tool is not yet been installed
        # within the tools directory, its sources are downloaded and it is installed.
        @staticmethod
        def get(name: str, target_triplet: str=None) -> str:
            from os.path import isfile
            if target_triplet is None or target_triplet == "":
                file = name
            else:
                file = target_triplet + "-" + name
            path = _Directory.tool.get("bin").join(file)
            if not isfile(path):
                _BuildInfo.Tool.__executables[name].build(target_triplet)
            return path
        
        
        ##
        # Moves all log files from the build_dir and all sub-directories to
        # the "logs" directory.
        @staticmethod
        def __save_logs() -> None:
            from logging import info
            from os import rename
            from os.path import dirname
            info("saving log files to logs/*")
            build_dir = _BuildInfo.Tool.get_build_dir()
            for log in build_dir.get_files([".log"]):
                rel_path = build_dir.rel_path(log)
                dst = _Directory.logs.join(rel_path)
                _Directory.logs.enforce(dirname(dst))
                rename(log, dst)
         
         
    ##
    # Downloads an URL to "tools/src".
    @staticmethod
    def _download(url_str: str) -> str:
        from logging import info, warning
        from os.path import basename, isfile
        from urllib.request import urlopen
        from urllib.parse import urlparse
        url = urlparse(url_str)
        output_file = _Directory.tool.enforce("repos").join(basename(url.path))
        with _TempFile(output_file) as tmp:
            if not tmp.exists:
                info("downloading " + url_str)
                if url.scheme != "https":
                    warning("protocol of " + url_str + " is not secured; consider using https instead")
                source = urlopen(url_str)
                while True:
                    data = source.read(4096)
                    if len(data) == 0:
                        break
                    tmp.write(data)
        return output_file
    
    
    ##
    # Invokes a function for all target platforms. The function takes the
    # targeted platform as its only parameter. It returns a list of generated
    # files.
    @staticmethod
    def invokeForAllPlatforms(delegates: list) -> list:
        assert(isinstance(delegates, list))
        _BuildInfo.__read()
        result = []
        for delegate in delegates:
            for platform in _BuildInfo.__platforms.values():
                files = delegate(platform, _ReadOnlyList(result))
                if files is not None:
                    result += files
        return result
    
    
    ##
    # Reads the contents of the build_info.xml file and validates them using DTD,
    # if required.
    @staticmethod
    def __read():
        from lxml import etree
        if _BuildInfo.__doc is not None:
            return
        file = _Directory.src.get('buildinfo.xml').path
        parser = etree.XMLParser(dtd_validation=True, remove_blank_text=True)
        _BuildInfo.__doc = etree.parse(file, parser)
        _BuildInfo._create("platform", _BuildInfo.Platform, _BuildInfo.__platforms, _BuildInfo.__doc)
        _BuildInfo._create("tool", _BuildInfo.Tool, None, _BuildInfo.__doc)
        _BuildInfo._create("signature", _BuildInfo.Signature, _BuildInfo.__signatures, _BuildInfo.__doc)
    
    
    ##
    # Creates a set of named objects from an XML root.
    @staticmethod
    def _create(tag: str, element_type: type, result: dict, root) -> None:
        assert(isinstance(tag, str))
        assert(isinstance(element_type, type(_BuildInfo)))
        assert(isinstance(result, (dict, list, type(None))))
        for element in root.findall(tag):
            current = element_type(element)
            if isinstance(result, dict):
                result.update({current.name : current})
            elif result is not None:
                result.append(current)
    

    ##
    # Returns the home directory, that is used for pgp signatures.
    @staticmethod
    def get_gpg_dir() -> _Directory:
        return _Directory.tool.enforce("gpg")


    ##
    # Invoke GPG
    @staticmethod
    def __gpg(parameters: list) -> None:
        command_line = ["gpg", "--homedir", ".", "--verbose"]
        _invoke(command_line + parameters, _BuildInfo.get_gpg_dir())
    
    
    ##
    # Verifies a downloaded file by downloading the corresponding signature
    # and invoking gpg --verify
    def _verify(url: str, local_path: str):
        from logging import info
        from os.path import basename
        from urllib.parse import urlparse
        _BuildInfo.__read()
        info("verifying " + basename(local_path))
        url = urlparse(url)
        uri = url.netloc + url.path
        for candidate in _BuildInfo.__signatures:
            if not uri.startswith(candidate.uri):
                continue
            signature = _BuildInfo._download(url.scheme + "://" + uri + candidate.extension)
            if candidate.public_key is not None:
                _BuildInfo.__gpg(["--keyserver", "pgp.mit.edu", "--recv-keys", candidate.public_key])
            if candidate.key_ring is None:
                keyring = []
            else:
                keyfile = _BuildInfo.get_gpg_dir().rel_path(_BuildInfo._download(candidate.key_ring))
                keyring = ["--keyring", keyfile]
            _BuildInfo.__gpg(keyring + ["--verify", _BuildInfo.get_gpg_dir().rel_path(signature)])
            return
        raise Exception("No signature definition for " + uri)


##
# Link all object files, if they have been changed. For each sub-folder in the
# architecture dependent "obj" folder, an artifact is created. The result is
# stored in a architecture dependent sub-folder within the "bin" folder. If the
# "src" folder contains a linker script for the artifact, it is used. The
# linker script must have the name link.ld.
#
# Examples:
#
# input files         | output file       | linker script
# --------------------|-------------------+-------------------
# obj/i386/kernel/*.o | bin/i386/kernel   | src/kernel/link.ld
# obj/rasppi/libc/*.o | bin/rasppi/libc.a | src/libc/link.ld
#
# The required binutils are downloaded to "tools/src" and installed to
# "tools/bin".
#
def __link(platform: _BuildInfo.Platform, generated_files: _ReadOnlyList) -> list:
    from logging import info
    from os.path import isfile
    result = []
    obj_dir = _Directory.obj.get(platform.name)
    bin_dir = _Directory.bin.enforce(platform.name)
    linker = platform.get_linker()
    info("linking for " + platform.name)
    obj_files = _create_tree_from_paths([obj_dir.rel_path(file) for file in generated_files if obj_dir.within(file)])
    for name in obj_files.keys():
        dir = obj_dir.get(name)
        output_file = bin_dir.join(name)
        map_file = output_file + ".map"
        command_line = [
            dir.rel_path(linker),
            "-o", dir.rel_path(output_file),
            "-M=" + dir.rel_path(map_file),
            "--nostdlib",
            "--strip-all"]
        for linker_symbol in platform.linker_symbols:
            command_line += ["--defsym", linker_symbol.name + "=" + linker_symbol.value]
        command_line += _get_paths_from_tree(obj_files[name])
        script = _Directory.src.get(name).join("link.ld")
        if isfile(script):
            command_line += ["-T", dir.rel_path(script)]
            for line in open(script).readlines():
                line = line.strip()
                if line.startswith("INPUT(") and line.endswith(")"):
                    raise Exception(line + " statement not yet supported")
        _invoke(command_line, dir)
        result += [output_file, map_file]
    return result


##
# Compile all source code files. Source code files are located within
# the "src" folder and have one of the following extensions:
# - .S for assembler files processed by the c preprocessor
# - .cpp for C++ files
def __compile(platform: _BuildInfo.Platform, generated_files: _ReadOnlyList) -> list:
    
    
    ##
    # Returns the path of the .o file, that is compiled from a given
    # source_file on a given platform. Returns None, if
    # - either the source_file is not compiled for the platform (i.e.
    #   the file extension does not contain .'platform.name'),
    # - or the output_file already exists and is newer than the source
    #   file and all its includes.
    def _derive_output_file(platform: _BuildInfo.Platform, source_file: str) -> str:
        from os.path import splitext
        rel_path, ext = splitext(_Directory.src.rel_path(source_file))
        obj_dir = _Directory.obj.enforce(platform.name)
        target_platform = splitext(rel_path)[1]
        output_file = obj_dir.join(rel_path) + ".o"
        if target_platform != "" and target_platform != "." + platform.name:
            return None
        return output_file
    
    
    ##
    # Checks, wether a file needs to be compiled.
    def _needs_compilation(platform: _BuildInfo.Platform, output_file: str, source_file: str) -> bool:
        from os.path import dirname, isfile
        if output_file is None:
            return False
        _Directory(dirname(output_file)).enforce()
        if not isfile(output_file):
            return True
        includes = platform.read_includes(source_file)
        return _needs_update(output_file, includes + [source_file])
    
    
    from logging import info
    compiler = platform.get_compiler()
    info("compiling for " + platform.name)
    result = []
    for source_file in _Directory.src.get_files([".S", ".cpp"]):
        output_file = _derive_output_file(platform, source_file)
        if output_file is not None:
            result.append(output_file)
        if not _needs_compilation(platform, output_file, source_file):
            continue
        command_line = [
            compiler,
            "-c", source_file,
            "-nostdinc",
            "-o", output_file,
            "-fno-rtti",
            "-DVERBOSE",
        ]
        if output_file.endswith(".bc"):
            command_line += ["-emit-llvm"]
        else:
            command_line += ["--target=" + platform.target_triplet] + platform.compiler_params
        if source_file.endswith(".cpp"):
            command_line += ["--std=c++11"]
        for include in platform.include_dirs:
            command_line += ["-I", include.path]
        _invoke(command_line)
    return result

 
##
# Test the builds.
def __test(platform: _BuildInfo.Platform, generated_files: _ReadOnlyList) -> None:
    from logging import info
    info("testing on " + platform.name)
    command_line = [
        platform.qemu,
        "-kernel", _Directory.logs.rel_path(_Directory.bin.get(platform.name).join("kernel")),
        #"-serial", "file: test-" + platform.name + ".log"
        #"-initrd", join(_bin_dir, platform.name, "launcher")
    ] + platform.qemu_params
    _invoke(command_line, _Directory.logs)


##
# Forms a directory tree from a list of paths.
def _create_tree_from_paths(paths: list) -> dict:
    from os import sep 
    result = {}
    for path in paths:
        current = result
        while path != "":
            try:
                pos = path.index(sep)
                name = path[:pos]
                path = path[pos+1:]
            except ValueError:
                name = path
                path = ""
            if name not in current:
                current.update({name: {}})
            current = current[name]
    return result


def _get_paths_from_tree(tree: dict) -> list:
    
    
    def _append(result: list, current: dict, prefix: str) -> None:
        from os.path import join
        for name in sorted(current.keys()):
            if prefix is None:
                path = name
            else:
                path = join(prefix, name)
            result.append(path)
            _append(result, current[name], path)
    
    
    result = []
    _append(result, tree, None)
    return result


_CommandLine.evaluate()
__init_logging()
_Directory.__static_init__()
result = _BuildInfo.invokeForAllPlatforms([__compile, __link, __test])
_Directory.obj.erase_contents(result)
_Directory.bin.erase_contents(result)
