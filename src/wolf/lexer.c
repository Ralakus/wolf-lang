#include "lexer.h"

#include <string.h>

void wolf_lexer_init(wolf_lexer_t* this, const char* source) {
    this->start   = source;
    this->current = source;
    this->line    = 1;
}

void wolf_lexer_free(wolf_lexer_t* this) {
    wolf_lexer_init(this, NULL);
}

static inline bool is_at_end(wolf_lexer_t* this) {
    return *this->current == '\0';
}

static inline bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static inline bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

static inline wolf_token_t make_token(wolf_lexer_t* this, wolf_token_type_t type) {
    return (wolf_token_t){
        .type = type,
        .data = this->start,
        .len  = this->current - this->start,
        .line = this->line
    };
}

static inline wolf_token_t error_token(wolf_lexer_t* this, const char* message) {
    return (wolf_token_t){
        .type = WOLF_TOK_ERR,
        .data = message,
        .len  = strlen(message),
        .line = this->line
    };
}

static inline char advance(wolf_lexer_t* this) {
    ++this->current;
    return this->current[-1];
}

static inline bool match(wolf_lexer_t* this, char expected) {
    if(is_at_end(this))            return false;
    if(*this->current != expected) return false;

    ++this->current;
    return true;
}

static inline char peek(wolf_lexer_t* this) {
    return *this->current;
}

static inline char peek_next(wolf_lexer_t* this) {
    if(is_at_end(this)) return '\0';
    return this->current[1];
}

static inline char peek_at_depth(wolf_lexer_t* this, int depth) {
    if(is_at_end(this)) return '\0';
    return this->current[depth];
}

static inline void skip_whitespace(wolf_lexer_t* this) {
    for(;;) {
        char c = peek(this);
        switch(c) {

            case '\n':
                ++this->line;

            case ' ' :
            case '\r':
            case '\t':
                advance(this);
                break;

            case '#': {
                while(peek(this) != '\n' && !is_at_end(this)) advance(this);
            } break;

            default:
                return;
        }
    }
}
static inline wolf_token_t string(wolf_lexer_t* this) {
    while(peek(this) != '\"' && !is_at_end(this)) {
        if(peek(this) == '\n') ++this->line;
        advance(this);
    }

    if(is_at_end(this)) return error_token(this, "Unterminated string");

    advance(this);

    return make_token(this, WOLF_TOK_STRING);
}

static inline wolf_token_t number(wolf_lexer_t* this) {
    while(is_digit(peek(this))) advance(this);

    if(peek(this) == '.' && is_digit(peek_next(this))) {
        advance(this);

        while(is_digit(peek(this))) advance(this);
    }

    return make_token(this, WOLF_TOK_NUMBER);
}

#define KEYWORD(str, tok, depth)                                     \
    if(this->current - this->start - depth == sizeof(str) - 1) {     \
        if(memcmp(this->start + depth, str, sizeof(str) - 1) == 0) { \
            return tok;                                              \
        }                                                            \
    }

static wolf_token_type_t match_identifier(wolf_lexer_t* this) {
    switch (this->start[0]) {
        default: {
        } break;

        case 'a': {
            KEYWORD("nd", WOLF_TOK_KW_AND, 1);
        } break;

        case 'b': {
            KEYWORD("reak", WOLF_TOK_KW_BREAK, 1);
        } break;

        case 'e': {
            KEYWORD("lse", WOLF_TOK_KW_ELSE, 1);
        } break;

        case 'f': {
            if(this->current - this->start > 1) {
                switch (this->start[1]) {
                    case 'a': {
                        KEYWORD("lse", WOLF_TOK_KW_FALSE, 2);
                    } break;

                    case 'o': {
                        KEYWORD("r", WOLF_TOK_KW_FOR, 2);
                    } break;
                }
            }
        } break;

        case 'i': {
            KEYWORD("f", WOLF_TOK_KW_IF, 1);
        } break;

        case 'o': {
            KEYWORD("r", WOLF_TOK_KW_OR, 1);
        } break;

        case 'r': {
            KEYWORD("eturn", WOLF_TOK_KW_RETURN, 1);
        } break;

        case 's': {
            if(this->current - this->start > 1) {
                switch (this->start[1]) {
                    case 'e': {
                        KEYWORD("lf", WOLF_TOK_KW_SELF, 2);
                    } break;

                    case 't': {
                        KEYWORD("ruct", WOLF_TOK_KW_STRUCT, 2);
                    }
                }
            }
        } break;

        case 't': {
            KEYWORD("rue", WOLF_TOK_KW_TRUE, 1);
        } break;

        case 'w': {
            KEYWORD("hile", WOLF_TOK_KW_WHILE, 1);
        } break;
    }
    return WOLF_TOK_INDENTIFIER;
}

static inline wolf_token_t identifier(wolf_lexer_t* this) {
    while(is_alpha(peek(this)) || is_digit(peek(this))) advance(this);

    return make_token(this, match_identifier(this));
}

wolf_token_t wolf_lexer_scan(wolf_lexer_t* this) {
    skip_whitespace(this);
    this->start = this->current;

    if(is_at_end(this)) return make_token(this, WOLF_TOK_EOF);

    char c = advance(this);
    if(is_digit(c)) return number(this);
    if(is_alpha(c)) return identifier(this);

    switch(c) {
        case '(': return make_token(this, WOLF_TOK_LPAREN);
        case ')': return make_token(this, WOLF_TOK_RPAREN);
        case '{': return make_token(this, WOLF_TOK_LCURLY);
        case '}': return make_token(this, WOLF_TOK_RCURLY);
        case '[': return make_token(this, WOLF_TOK_LBRACKET);
        case ']': return make_token(this, WOLF_TOK_RBRACKET);

        case ',': return make_token(this, WOLF_TOK_COMMA);
        case '.': return make_token(this, WOLF_TOK_DOT);
        case ';': return make_token(this, WOLF_TOK_SEMICOLON);
        
        case '+': return make_token(this, WOLF_TOK_PLUS);
        case '-': return make_token(this, WOLF_TOK_MINUS);
        case '*': return make_token(this, WOLF_TOK_STAR);
        case '/': return make_token(this, WOLF_TOK_SLASH);

        case '!':
            return make_token(this, match(this, '=') ? WOLF_TOK_BANG_EQUAL    : WOLF_TOK_BANG);
        case '=':
            return make_token(this, match(this, '=') ? WOLF_TOK_EQUAL_EQUAL   : WOLF_TOK_EQUAL);
        case '<':
            return make_token(this, match(this, '=') ? WOLF_TOK_LESS_EQUAL    : WOLF_TOK_LESS);
        case '>':
            return make_token(this, match(this, '=') ? WOLF_TOK_GREATER_EQUAL : WOLF_TOK_GREATER);
        case ':': 
            return make_token(this, match(this, ':') ? WOLF_TOK_COLON_COLON   : WOLF_TOK_COLON);

        case '\"': return string(this);
    }

    return error_token(this, "Unexpected character");
}

const char* wolf_token_str_map[] = {

    /*
        Single char tokens
    */
    "(",
    ")",
    "{",
    "}",
    "[",
    "]",

    ",",
    ".",
    ";",

    "+",
    "-",
    "*",
    "/",

    /*
        One or two char tokens
    */
    "!",
    "!=",
    "=",
    "==",
    ">",
    ">=",
    "<",
    "<=",

    ":",
    "::",


    /*
        Literals
    */
    "identifier",
    "string",
    "interpolation",
    "number",

    /*
        Keywords
    */
    "and",
    "or",

    "self",
    "struct",
    "return",

    "if",
    "else",
    "while",
    "for",
    "break",

    "true",
    "false",

    /*
        Special
    */
    "<err>",
    "eof",
};