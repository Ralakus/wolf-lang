#pragma once

#include <stdbool.h>

typedef struct {

    bool debug_mode;

} wolf_repl_params_t;

/* runs repl with params */
bool wolf_repl(wolf_repl_params_t* params);