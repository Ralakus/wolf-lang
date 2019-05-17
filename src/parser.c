#include "parser.h"

#include "logger.h"

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
    this->was_error = false;
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

static inline void expression(wolf_parser_t* this) {

}

bool wolf_parser_parse(wolf_parser_t* this, wolf_bytecode_t* bytecode, const char* source) {
    wolf_lexer_init(&this->lexer, source);
    this->was_error  = false;
    this->panic_mode = false;
    this->bytecode   = bytecode;

    advance(this);
    expression(this);
    consume(this, WOLF_TOK_EOF, "Expected end of expression");

    end_parse(this);

    wolf_lexer_free(&this->lexer);
    return !this->was_error;
}