#include "parser.h"

#include "util/logger.h"

#include <stdlib.h>
#include <string.h>


void wolf_parser_init(wolf_parser_t* this) {
    wolf_lexer_init(&this->lexer, NULL);
    this->was_error  = false;
    this->panic_mode = false;
    this->debug_mode = false;
    this->line       = -1;
    this->bytecode   = NULL;
}

void wolf_parser_free(wolf_parser_t* this) {
    wolf_parser_init(this);
}


static inline wolf_bytecode_t* current_bytecode(wolf_parser_t* this) {
    return this->bytecode;
}

static void error_at(wolf_parser_t* this, wolf_token_t* token, const char* message) {
    if(this->panic_mode) return;
    this->panic_mode = true;

    wolf_error("[line: %d]: error", token->line);

    if(token->type == WOLF_TOK_EOF) {
        wolf_error_raw(" at end");
    } else if(token->type == WOLF_TOK_ERR) {
        // Nothing since it's handled elsewhere
    } else {
        wolf_error_raw(" at \'%.*s\'", token->len, token->data);
    }

    wolf_errorln_raw(": %s", message);
    this->was_error = true;
}

static void error_at_current(wolf_parser_t* this, const char* message) {
    error_at(this, &this->current, message);
}

static void error(wolf_parser_t* this, const char* message) {
    error_at(this, &this->previous, message);
}

static inline void advance(wolf_parser_t* this) {
    this->previous = this->current;

    for(;;) {
        this->current = wolf_lexer_scan(&this->lexer);

        if(this->debug_mode) {
            if(this->current.line != this->line) {
                wolf_print_raw(WOLF_ANSI_RED"%4d "WOLF_ANSI_RESET, this->current.line);
                this->line = this->current.line;
            } else {
                wolf_print_raw(WOLF_ANSI_RED"   | "WOLF_ANSI_RESET);
            }
            if(this->current.type == WOLF_TOK_EOF) {
                wolf_println_raw(WOLF_ANSI_GREEN"%-14.14s "WOLF_ANSI_YELLOW"\'\\0\'"WOLF_ANSI_RESET, wolf_token_str_map[this->current.type]);
                break;
            }
            wolf_println_raw(
                WOLF_ANSI_GREEN"%-14.14s"
                WOLF_ANSI_YELLOW" \'%.*s\'"
                WOLF_ANSI_RESET, 
                    wolf_token_str_map[this->current.type], 
                    this->current.len, 
                    this->current.data
            );
        }

        if(this->current.type != WOLF_TOK_ERR) break;

        error_at_current(this, this->current.data);
    }
}

static inline void consume(wolf_parser_t* this, wolf_token_type_t type, const char* message) {
    if(this->current.type == type) {
        advance(this);
        return;
    } else {
        error_at_current(this, message);
    }
}

static inline void emit_byte(wolf_parser_t* this, uint8_t byte) {
    wolf_bytecode_write(current_bytecode(this), byte, this->previous.line);
}

static inline void emit_bytes(wolf_parser_t* this, uint8_t byte1, uint8_t byte2) {
    emit_byte(this, byte1);
    emit_byte(this, byte2);
}

static inline void emit_return(wolf_parser_t* this) {
    emit_byte(this, WOLF_OP_RETURN);
}

static inline void end_parse(wolf_parser_t* this) {
    emit_return(this);
}

static inline uint16_t make_constant(wolf_parser_t* this, wolf_value_t value) {
    for(isize_t i = 0; i < current_bytecode(this)->constants.len; i++) {
        if(wolf_value_is_equal(current_bytecode(this)->constants.values[i], value)) {
            return (uint16_t)i;
        }
    }
    int constant = wolf_bytecode_write_constant(current_bytecode(this), value);
    if (constant > UINT16_MAX) {
        error(this, "Too many constants in one chunk");
        return 0;
    }

    return (uint16_t)constant;
}

static inline void emit_constant(wolf_parser_t* this, wolf_value_t value) {
    uint16_t index = make_constant(this, value);
    if(index <= UINT8_MAX) {
        emit_bytes(this, WOLF_OP_CONSTANT, (uint8_t)index);
    } else {
        emit_byte(this, WOLF_OP_CONSTANT_L);
        emit_bytes(this, ((uint8_t*)&index)[0], ((uint8_t*)&index)[1]);
    }
}



static inline void expression (wolf_parser_t* this);
static inline void number     (wolf_parser_t* this);
static inline void grouping   (wolf_parser_t* this);
static inline void unary      (wolf_parser_t* this);
static inline void binary     (wolf_parser_t* this);
static inline void literal    (wolf_parser_t* this);

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_UNARY,       // ! -
  PREC_CALL,        // . () []
  PREC_PRIMARY
} precedence_t;

typedef void(*parse_fn_t)(wolf_parser_t*);

typedef struct {
    parse_fn_t prefix;
    parse_fn_t infix;
    precedence_t precedence;
} parse_rule_t;

parse_rule_t rules[] = {
    { grouping, NULL,   PREC_CALL       }, // WOLF_TOK_LPAREN
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_RPAREN
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_LCURLY
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_RCURLY
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_LBRACKET
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_RBRACKET
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_COMMA
    { NULL,     NULL,   PREC_CALL       }, // WOLF_TOK_DOT
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_SEMICOLON
    { NULL,     binary, PREC_TERM       }, // WOLF_TOK_PLUS
    { unary,    binary, PREC_TERM       }, // WOLF_TOK_MINUS
    { NULL,     binary, PREC_FACTOR     }, // WOLF_TOK_STAR
    { NULL,     binary, PREC_FACTOR     }, // WOLF_TOK_SLASH
    { unary,    NULL,   PREC_NONE       }, // WOLF_TOK_BANG
    { NULL,     binary, PREC_EQUALITY   }, // WOLF_TOK_BANG_EQUAL
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_EQUAL
    { NULL,     binary, PREC_EQUALITY   }, // WOLF_TOK_EQUAL_EQUAL
    { NULL,     binary, PREC_COMPARISON }, // WOLF_TOK_GREATER
    { NULL,     binary, PREC_COMPARISON }, // WOLF_TOK_GREATER_EQUAL
    { NULL,     binary, PREC_COMPARISON }, // WOLF_TOK_LESS
    { NULL,     binary, PREC_COMPARISON }, // WOLF_TOK_LESS_EQUAL
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_COLON
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_COLON_COLON
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_INDENTIFIER
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_STRING
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_INTERPOLATION
    { number,   NULL,   PREC_NONE       }, // WOLF_TOK_NUMBER
    { NULL,     NULL,   PREC_AND        }, // WOLF_TOK_KW_AND
    { NULL,     NULL,   PREC_OR         }, // WOLF_TOK_KW_OR
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_KW_SELF
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_KW_STRUCT
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_KW_RETURN
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_KW_IF
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_KW_ELSE
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_KW_WHILE
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_KW_FOR
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_KW_BREAK
    { literal,  NULL,   PREC_NONE       }, // WOLF_TOK_KW_TRUE
    { literal,  NULL,   PREC_NONE       }, // WOLF_TOK_KW_FALSE
    { literal,  NULL,   PREC_NONE       }, // WOLF_TOK_KW_NIL
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_ERR
    { NULL,     NULL,   PREC_NONE       }, // WOLF_TOK_EOF
};


static parse_rule_t* get_rule(wolf_token_type_t type) {
    return &rules[type];
}


static inline void parse_precedence(wolf_parser_t* this, precedence_t precedence) {
    advance(this);
    parse_fn_t prefix_rule = get_rule(this->previous.type)->prefix;

    if(prefix_rule == NULL) {
        error(this, "Expect expression");
        return;
    }

    prefix_rule(this);

    while (precedence <= get_rule(this->current.type)->precedence) {
        advance(this);
        parse_fn_t infix_rule = get_rule(this->previous.type)->infix;
        if(infix_rule == NULL) {
            error(this, "Expected infix operator!");
            return;
        }
        infix_rule(this);
    }
}




static inline void expression(wolf_parser_t* this) {
    parse_precedence(this, PREC_ASSIGNMENT);
}

static inline void number(wolf_parser_t* this) {
    double value = strtod(this->previous.data, NULL);
    emit_constant(this, WOLF_VALUE_NUMBER(value));
}

static inline void grouping(wolf_parser_t* this) {
    expression(this);
    consume(this, WOLF_TOK_RPAREN, "Expect \')\' after expression");
}

static inline void unary(wolf_parser_t* this) {
    wolf_token_type_t op_type = this->previous.type;

    parse_precedence(this, PREC_UNARY);

    switch (op_type) {
        case WOLF_TOK_BANG: 
            emit_byte(this, WOLF_OP_NOT);
            break;
        case WOLF_TOK_MINUS:
            emit_byte(this, WOLF_OP_NEGATE);
            break;
        default:
            UNREACHABLE();
    }
}

static void binary(wolf_parser_t* this) {
    wolf_token_type_t op_type = this->previous.type;

    parse_rule_t* rule = get_rule(op_type);
    parse_precedence(this, (precedence_t)(rule->precedence + 1));

    switch (op_type) {
        case WOLF_TOK_PLUS:
            emit_byte(this, WOLF_OP_ADD);
            break;
        case WOLF_TOK_MINUS:
            emit_byte(this, WOLF_OP_SUB);
            break;
        case WOLF_TOK_STAR:
            emit_byte(this, WOLF_OP_MUL);
            break;
        case WOLF_TOK_SLASH:
            emit_byte(this, WOLF_OP_DIV);
            break;

        case WOLF_TOK_BANG_EQUAL:
            emit_bytes(this, WOLF_OP_EQUAL, WOLF_OP_NOT);
            break;
        case WOLF_TOK_EQUAL_EQUAL: 
            emit_byte(this, WOLF_OP_EQUAL);
            break;
        case WOLF_TOK_GREATER:
            emit_byte(this, WOLF_OP_GREATER);
            break;
        case WOLF_TOK_GREATER_EQUAL:
            emit_bytes(this, WOLF_OP_LESS, WOLF_OP_NOT);
            break;
        case WOLF_TOK_LESS:
            emit_byte(this, WOLF_OP_LESS);
            break;
        case WOLF_TOK_LESS_EQUAL:
            emit_bytes(this, WOLF_OP_GREATER, WOLF_OP_NOT);
            break;
        default:
            UNREACHABLE();
    }
}

static inline void literal(wolf_parser_t* this) {
    switch(this->previous.type) {
        case WOLF_TOK_KW_TRUE:
            emit_byte(this, WOLF_OP_TRUE);
            break;
        case WOLF_TOK_KW_FALSE: 
            emit_byte(this, WOLF_OP_FALSE);
            break;
        case WOLF_TOK_KW_NIL:
            emit_byte(this, WOLF_OP_NIL);
            break;
        default:
            UNREACHABLE();
    }
}









bool wolf_parser_parse(wolf_parser_t* this, wolf_bytecode_t* bytecode, const char* source) {
    wolf_lexer_init(&this->lexer, source);
    this->was_error  = false;
    this->panic_mode = false;
    this->bytecode   = bytecode;

    if(this->debug_mode) {
        wolf_noticeln(WOLF_ANSI_CYAN"--== Tokens ==--"WOLF_ANSI_RESET);
    }

    advance(this);
    expression(this);
    consume(this, WOLF_TOK_EOF, "Expected end of expression");

    end_parse(this);

    wolf_lexer_free(&this->lexer);
    return !this->was_error;
}