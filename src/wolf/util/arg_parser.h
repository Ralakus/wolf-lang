#pragma once

#include "common.h"

/* ================================

    Argument type

================================ */ 

typedef struct {
    const char* short_name;
    const char* long_name;
    const char* description;

    bool expect_preceeding;

    bool found;

    isize_t pa_alloc_len;
    isize_t pa_len;
    const char** preceeding_args;
} wolf_arg_t;

/* inits arg, must be done before use */
void wolf_arg_init(wolf_arg_t* arg, const char* short_name, const char* long_name, const char* description, bool expect_preceeding);
/* frees arg, must be done after use */
void wolf_arg_free(wolf_arg_t* arg);

/* ================================

    Argument parser

================================ */ 

typedef struct {
    isize_t      a_alloc_len;
    isize_t      a_len;
    wolf_arg_t** args;

    isize_t      ea_alloc_len;
    isize_t      ea_len;
    const char** extra_args;
} wolf_arg_parser_t;

/* inits arg parser, must be done before use */
void wolf_arg_parser_init   (wolf_arg_parser_t* parser);
/* frees arg parser, must be done after use */
void wolf_arg_parser_free   (wolf_arg_parser_t* parser);

/* adds argument to arg parser, does not free pointer */
void wolf_arg_parser_add_arg(wolf_arg_parser_t* parser, wolf_arg_t* arg);
/* parses program arguments into wolf_arg_t */
bool wolf_arg_parser_parse  (wolf_arg_parser_t* parser, int argc, const char** argv);

/* prints a generated help message */
void wolf_arg_parser_print_help(wolf_arg_parser_t* parser, const char* program_description);