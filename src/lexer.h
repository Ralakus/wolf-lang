#pragma once

#include "util/common.h"

typedef enum {
    WOLF_TOK
} wolf_token_type_t;

typedef struct {
    const char* start;
    const char* current;
    isize_t line;
} wolf_lexer_t;

typedef struct {
    wolf_token_type_t type;
    const char* data;
    isize_t len;
    isize_t line;
} wolf_token_t;