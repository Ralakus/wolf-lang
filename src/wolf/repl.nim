
type
  replParams* {.bycopy, header: "\"wolf/repl.h\"", importc: "wolf_repl_params_t".} = object
    debugMode*{.importc: "debug_mode".}: bool


##  runs repl with params
proc repl*(params: ptr replParams): bool {.header: "\"wolf/repl.h\"", importc: "wolf_repl".}