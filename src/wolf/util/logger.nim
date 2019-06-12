##
##
##     Five types of print function each with a newline varient:
##         print
##         notice
##         warn
##         error
##         success
##
##

{.push header: "\"util/logger.h\"".}

when not defined(WOLF_NO_ANSI_COLOUR):
  const
    ANSI_RED* = "\e[31m"
    ANSI_GREEN* = "\e[32m"
    ANSI_YELLOW* = "\e[33m"
    ANSI_BLUE* = "\e[34m"
    ANSI_MAGENTA* = "\e[35m"
    ANSI_CYAN* = "\e[36m"
    ANSI_RESET* = "\e[0m"
else:
  const
    ANSI_RED* = ""
    ANSI_GREEN* = ""
    ANSI_YELLOW* = ""
    ANSI_BLUE* = ""
    ANSI_MAGENTA* = ""
    ANSI_CYAN* = ""
    ANSI_RESET* = ""


import
  common

##  returns a FILE*
proc printStream*(): pointer {.importc: "wolf_print_stream".}

##  returns a FILE*
proc errorStream*(): pointer {.importc: "wolf_error_stream".}

##   stream must be a FILE*!
proc retargetPrint*(stream: pointer): bool {.importc: "wolf_retarget_print".}

##  stream must be a FILE*!
proc retargetError*(stream: pointer): bool {.importc: "wolf_retarget_error".}

##  outputs to print stream, prefixed with '[ ]: '
proc rint*(fmt: cstring) {.varargs, importc: "wolf_print".}

##  outputs to print stream, prefixed with '[ ]: ' and newlined
proc println*(fmt: cstring) {.varargs, importc: "wolf_println".}

##  outputs to print stream, prefixed with '[-]: '
proc notice*(fmt: cstring) {.varargs, importc: "wolf_notice".}

##  outputs to print stream, prefixed with '[-]: ' and newlined
proc noticeln*(fmt: cstring) {.varargs, importc: "wolf_noticeln".}

##  outputs to error stream, prefixed with '[*]: '
proc warn*(fmt: cstring) {.varargs, importc: "wolf_warn".}

##  outputs to error stream, prefixed with '[*]: ' and newlined
proc warnln*(fmt: cstring) {.varargs, importc: "wolf_warnln".}

##  outputs to error stream, prefixed with '[!]: '
proc error*(fmt: cstring) {.varargs, importc: "wolf_error".}

##  outputs to error stream, prefixed with '[!]: ' and newlined
proc errorln*(fmt: cstring) {.varargs, importc: "wolf_errorln".}

##  outputs to print stream, prefixed with '[^]: '
proc success*(fmt: cstring) {.varargs, importc: "wolf_success".}

##  outputs to print stream, prefixed with '[^]: ' and newlined
proc successln*(fmt: cstring) {.varargs, importc: "wolf_successln".}

##  outputs to print stream
proc printRaw*(fmt: cstring) {.varargs, importc: "wolf_print_raw".}

##  outputs to print stream and newlined
proc printlnRaw*(fmt: cstring) {.varargs, importc: "wolf_println_raw".}

##  outputs to error stream
proc errorRaw*(fmt: cstring) {.varargs, importc: "wolf_error_raw".}

##  outputs to print stream and newlined
proc errorlnRaw*(fmt: cstring) {.varargs, importc: "wolf_errorln_raw".}

{.pop.}