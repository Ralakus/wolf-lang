import
  common

{.push header: "\"util/arg_parser.h\"".}

##  ================================
##
##     Argument type
##
## ================================

type
  arg* {.bycopy, importc: "wolf_arg_t".} = object
    short_name*: cstring
    long_name*: cstring
    description*: cstring
    expect_preceeding*: bool
    found*: bool
    pa_alloc_len*: isize
    pa_len*: isize
    preceeding_args*: cstringArray


##  inits arg, must be done before use
proc init*(arg: ptr arg; short_name: cstring; long_name: cstring;
                   description: cstring; expect_preceeding: bool) {.importc: "wolf_arg_init".}


##  frees arg, must be done after use
proc free*(arg: ptr arg) {.importc: "wolf_arg_free".}

##  ================================
##
##     Argument parser
##
## ================================

type
  argParser* {.bycopy, importc: "wolf_arg_parser_t".}= object
    a_alloc_len*: isize
    a_len*: isize
    args*: ptr ptr arg
    ea_alloc_len*: isize
    ea_len*: isize
    extra_args*: cstringArray


##  inits arg parser, must be done before use
proc init*(parser: ptr argParser) {.importc: "wolf_arg_parser_init".}

##  frees arg parser, must be done after use
proc free*(parser: ptr argParser) {.importc: "wolf_arg_parser_free".}

##  adds argument to arg parser, does not free pointer
proc addArg*(parser: ptr argParser; arg: ptr arg) {.importc: "wolf_arg_parser_add_arg".}

##  parses program arguments into wolf_arg_t
proc parse*(parser: ptr argParser; argc: cint;
                           argv: cstringArray): bool {.importc: "wolf_arg_parser_parse".}

##  prints a generated help message
proc printHelp*(parser: ptr argParser;
                                program_description: cstring) {.importc: "wolf_arg_parser_print_help".}

{.pop.}