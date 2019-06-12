
type
    wolf_t* {. header: "\"wolf/instance.h\"", importc: "wolf_t" .} = object

##  inits instance, must be done before use
proc init*(instance: ptr wolf_t) {. header: "\"wolf/instance.h\"", importc: "wolf_init" .}

##  frees instance, must be done after use
proc free*(instance: ptr wolf_t) {. header: "\"wolf/instance.h\"", importc: "wolf_free" .}

##  sets debug mode
proc setDebugMode*(instance: ptr wolf_t; value: bool) {. header: "\"wolf/instance.h\"", importc: "wolf_set_debug_mode" .}

##  loads and compiles a string of code
proc load*(instance: ptr wolf_t; code: cstring): bool {. header: "\"wolf/instance.h\"",importc: "wolf_load" .}

##  loads and compiles a file of code
proc loadFile*(instance: ptr wolf_t; file_name: cstring): bool {. header: "\"wolf/instance.h\"", importc: "wolf_load_file" .}

##  loads a bytecode file
proc loadBytecodeFile*(instance: ptr wolf_t; file_name: cstring): bool {. header: "\"wolf/instance.h\"", importc: "wolf_load_bytecode_file" .}

##  unloads loaded bytecode
proc unload*(instance: ptr wolf_t): bool {. header: "\"wolf/instance.h\"", importc: "wolf_unload" .}

##  runs loaded bytecode
proc run*(instance: ptr wolf_t): bool {. header: "\"wolf/instance.h\"", importc: "wolf_run" .}