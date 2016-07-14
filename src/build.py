#!/bin/python3

##
# @package  build
# The contents of the build script for Dr Grätz OS (for details refer to
# @ref build.py). The following temporary directories are created, if required:
# - logs: contains all log files, will be erased before every build
#
# @file
# The build script for Dr Grätz OS.
#
# @dir src
# Contains all source files.


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
# Annotation used for annotationg build targets.
class _Target:
   __all = {}
   __invoked = []
   all = _ReadOnlyDict(__all)
   
   
   ##
   # Tracks the execution of build tasks. This function is invoked instead of
   # the build task function. It will invoke the build task if (and only if)
   # it has not been previously executed.
   def __call__(self):
      if self not in _Target.__invoked:
         _Target.__invoked.append(self)
         self.__function()
   
   
   ##
   # Default constructor.
   def __init__(self, annotated_element):
      from inspect import isfunction
      if not isfunction(annotated_element) or \
         annotated_element.__code__.co_argcount != 0:
            raise Exception("@_Target annotation may only be used on "\
            "parameterless functions.")
      if annotated_element.__doc__ is None or \
         len(annotated_element.__doc__) == 0:
         raise Exception("@_Target annotation requires inline documentation.")
      name = annotated_element.__name__
      while name[0] == '_':
         name = name[1:]
      if name in _Target.__all:
         raise Exception("Another @_Target named " + name + \
         " has already been defined.")
      _Target.__all.update({name: self})
      self.__function = annotated_element
   
   
   ##
   # The documentation of the underlying function.
   @property
   def doc(self) -> str:
      return self.__function.__doc__


##
# Erases a file or directory (and all its contents) except for a list of files
# which shall not be deleted. If no exception list is provided, no exceptions
# are made. The erase machanisms distingushes between symbol links (which are
# unlinked), directories (which are recursively deleted), and files (which are
# removed). Raises an exception, if a file, which is not explicitly excluded,
# cannot be erased.
def _erase(path: str, exceptions: list=[]) -> None:
   from os import listdir, remove, rmdir, unlink, walk
   from os.path import isdir, isfile, islink, join
   assert(isinstance(path, str))
   assert(isinstance(exceptions, list))
   if path not in exceptions:
      if islink(path):
         unlink(path)
      elif isdir(path):
         for file in listdir(path):
            _erase(join(path, file), exceptions)
         if len(listdir(path)) == 0:
            rmdir(path)
      elif isfile(path):
         remove(path)


##
# Enforces the presence of a directory. If not already present, the directory
# and all required parent directories are created. Raises an exception, in case
# the directory cannot be created.
def _enforce_dir(path: str) -> None:
   from os import mkdir
   from os.path import dirname, isdir
   assert(isinstance(path, str))
   if path != "" and not isdir(path):
      _enforce_dir(dirname(path))
      mkdir(path)


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
   debug(command_line)
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
   from logging import DEBUG, ERROR, FileHandler, Formatter, getLogger, INFO, \
      LogRecord, WARNING, StreamHandler
   from os.path import basename, join, splitext
   from sys import argv
   
   
   ##
   # Formatter for the console output. Prints  the different log levels in
   # differnt colors (ERROR = red, WARNING = yellow, INFO = blue,
   # other = white) and trims the lines to the screen width.
   class __PrettyFormatter(Formatter):
      def __init__(self):
         Formatter.__init__(self)
      
      def format(self, record: LogRecord) -> str:
         from os import linesep
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
         result = "\033[" + str(30 + color)
         if bold:
            result += ";1"
         result += "m"
         n = 1
         for line in record.msg.splitlines(True):
            if n == 10:
               result += "  ..."
               break
            n += 1
            if len(line) > 76:
               result += line[0:76] + "..."
            else:
               result += line
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
   _erase("logs", ["logs"])
   _enforce_dir("logs")
   log_name = splitext(basename(argv[0]))[0] + ".log"
   file = FileHandler(join("logs", log_name), "w")
   file.setLevel(DEBUG)
   logger.addHandler(file)
   import sys
   sys.excepthook = __exception_handler


##
# Evaluates the contents of the command line and returns a list of build
# targets. Each build target is defined as a parameterless function.
def __evaluate_command_line() -> list:
   from logging import error
   from sys import argv
   global _use_bitcode
   result = []
   i = 1
   targets = _Target.all
   while i < len(argv):
      current = argv[i]
      if current in targets:
         result.append(targets[current])
      elif current == "--help" or current == "-?":
         __print_help()
      else:
         raise ValueError("Invalid command line option: " + current)
      i += 1
   if len(result) == 0:
      result = [__all]
   return result


##
# Prints an online help to stdout.
def __print_help() -> None:
   from sys import argv
   print("Usage: " + argv[0] + " [target [target [...]]] [options]")
   print()
   print("Valid targets are (default = all):")
   names = list(_Target.all.keys())
   names.sort()
   for name in names:
      descr = _Target.all[name].doc
      if descr is None:
         print(name)
      else:
         print(name + "\t" + descr)
   print()
   print("Valid options are:")
   print("--help, -? print this help and exit")
   exit()


##
# Creates the doxygen documentation. Logs a warning, if doxygen is not
# installed. The file "src/doxygen.config" is used as doxygen configuration.
@_Target
def __doc() -> None:
   """Create the doxgen documentation."""
   from logging import info, warning
   info("creating documentation")
   _erase("doc", ["doc"])
   _enforce_dir("doc")
   try:
      _invoke(["doxygen", "src/doxygen.config"])
   except FileNotFoundError as e:
      warning("doxygen not installed - no documentation generated")


##
# Complete build. Includes:
# - @ref __doc()
@_Target
def __all() -> None:
   """Update all contents."""
   __doc()


__init_logging()
try:
   __targets = __evaluate_command_line()
except ValueError as e:
   from logging import error
   error(e.args[0])
   __print_help()
for __target in __targets:
   __target()
