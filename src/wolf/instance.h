#pragma once

#include "parser.h"
#include "vm.h"

typedef struct {
    bool            debug_mode;
    bool            loaded;
    bool            file_loaded;
    const char*     source;
    wolf_parser_t   parser;
    wolf_bytecode_t bytecode;
    wolf_vm_t       vm;
} wolf_t;

/* inits instance, must be done before use */
void wolf_init(wolf_t* instance);
/* frees instance, must be done after use */
void wolf_free(wolf_t* instance);

/* sets debug mode */
void wolf_set_debug_mode(wolf_t* instance, bool value);

/* loads and compiles a string of code */
bool wolf_load(wolf_t* instance, const char* code);
/* loads and compiles a file of code */
bool wolf_load_file(wolf_t* instance, const char* file_name);
/* loads bytecode and takes ownership over bytecode */
bool wolf_load_bytecode(wolf_t* instance, wolf_bytecode_t bytecode);
/* loads a bytecode file */
bool wolf_load_bytecode_file(wolf_t* instance, const char* file_name);

/* unloads loaded bytecode */
bool wolf_unload(wolf_t* instance);

/* runs loaded bytecode */
bool wolf_run(wolf_t* instance);
