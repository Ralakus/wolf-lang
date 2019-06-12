
type
  replParams* {.bycopy, header: "\"repl.h\"", importc: "wolf_repl_params_t".} = object
    debugMode*{.importc: "debug_mode".}: bool


##  runs repl with params
proc repl*(params: ptr replParams): bool {.header: "\"repl.h\"", importc: "wolf_repl".}