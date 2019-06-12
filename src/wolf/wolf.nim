
import vm


type
    wolfInstance* {.header: "\"wolf/instance.h\"", importc: "wolf_t".} = object
        bytecode* {.importc: "bytecode"}: wolf_bytecode_t
    
{.push header: "\"wolf/instance.h\"".}

##  inits instance, must be done before use
proc init*(instance: ptr wolfInstance) {.importc: "wolf_init".}

##  frees instance, must be done after use
proc free*(instance: ptr wolfInstance) {.importc: "wolf_free".}

##  sets debug mode
proc setDebugMode*(instance: ptr wolfInstance; value: bool) {.importc: "wolf_set_debug_mode".}

##  loads and compiles a string of code
proc load*(instance: ptr wolfInstance; code: cstring): bool {.importc: "wolf_load".}

##  loads and compiles a file of code
proc loadFile*(instance: ptr wolfInstance; file_name: cstring): bool {.importc: "wolf_load_file".}

##  loads a bytecode file
proc loadBytecodeFile*(instance: ptr wolfInstance; file_name: cstring): bool {.importc: "wolf_load_bytecode_file".}

##  unloads loaded bytecode
proc unload*(instance: ptr wolfInstance): bool {.importc: "wolf_unload".}

##  runs loaded bytecode
proc run*(instance: ptr wolfInstance): bool {. header: "\"wolf/instance.h\"", importc: "wolf_run" .}

{.pop.}