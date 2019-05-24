#pragma once

#include "util/common.h"

typedef enum {

    /*
        Single char tokens
    */
    WOLF_TOK_LPAREN,
    WOLF_TOK_RPAREN,
    WOLF_TOK_LCURLY,
    WOLF_TOK_RCURLY,
    WOLF_TOK_LBRACKET,
    WOLF_TOK_RBRACKET,

    WOLF_TOK_COMMA,
    WOLF_TOK_DOT,
    WOLF_TOK_SEMICOLON,

    WOLF_TOK_PLUS,
    WOLF_TOK_MINUS,
    WOLF_TOK_STAR,
    WOLF_TOK_SLASH,

    /*
        One or two char tokens
    */
    WOLF_TOK_BANG,
    WOLF_TOK_BANG_EQUAL,
    WOLF_TOK_EQUAL,
    WOLF_TOK_EQUAL_EQUAL,
    WOLF_TOK_GREATER,
    WOLF_TOK_GREATER_EQUAL,
    WOLF_TOK_LESS,
    WOLF_TOK_LESS_EQUAL,

    WOLF_TOK_COLON,
    WOLF_TOK_COLON_COLON,

    /*
        Literals
    */
    WOLF_TOK_INDENTIFIER,
    WOLF_TOK_STRING,
    WOLF_TOK_INTERPOLATION,
    WOLF_TOK_NUMBER,

    /*
        Keywords
    */
    WOLF_TOK_KW_AND,
    WOLF_TOK_KW_OR,

    WOLF_TOK_KW_SELF,
    WOLF_TOK_KW_STRUCT,
    WOLF_TOK_KW_RETURN,

    WOLF_TOK_KW_IF,
    WOLF_TOK_KW_ELSE,
    WOLF_TOK_KW_WHILE,
    WOLF_TOK_KW_FOR,
    WOLF_TOK_KW_BREAK,

    WOLF_TOK_KW_TRUE,
    WOLF_TOK_KW_FALSE,

    /*
        Special
    */
    WOLF_TOK_ERR,
    WOLF_TOK_EOF,
} wolf_token_type_t;

const char* wolf_token_str_map[WOLF_TOK_EOF + 1];

typedef struct {
    wolf_token_type_t type;
    const char* data;
    isize_t len;
    isize_t line;
} wolf_token_t;

typedef struct {
    const char* start;
    const char* current;
    isize_t line;
} wolf_lexer_t;

/* inits lexer, must be done before use */
void wolf_lexer_init(wolf_lexer_t* lexer, const char* source);
/* frees lexer, must be done after use */
void wolf_lexer_free(wolf_lexer_t* lexer);

/* scans for next token in loaded string */
wolf_token_t wolf_lexer_scan(wolf_lexer_t* lexer);

