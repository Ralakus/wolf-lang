#pragma once

#include "lexer.h"
#include "vm.h"

typedef struct {
    wolf_lexer_t     lexer;
    wolf_token_t     current;
    wolf_token_t     previous;
    bool             was_error;
    bool             panic_mode;
    wolf_bytecode_t* bytecode;
} wolf_parser_t;

/* inits parser, must be done before use */
void wolf_parser_init(wolf_parser_t* parser);
/* frees parser, must be onde after use */
void wolf_parser_free(wolf_parser_t* parser);

bool wolf_parser_parse(wolf_parser_t* parser, wolf_bytecode_t* bytecode, const char* source);