#pragma once

#include "util/common.h"


/* ================================

    Bytecode

================================ */ 

typedef enum {
    WOLF_OP_RETURN,
} wolf_opcode_t;

typedef struct {
    ptrdiff_t alloc_len;
    ptrdiff_t len;
    uint8_t* code;
} wolf_bytecode_t;

/* inits bytecode, must be done before use */
void wolf_bytecode_init(wolf_bytecode_t* bytecode);
/* frees bytecode, must be done after use */
void wolf_bytecode_free(wolf_bytecode_t* bytecode);

/* writes a single byte to bytecode */
void wolf_bytecode_write(wolf_bytecode_t* bytecode, uint8_t byte);

/* ================================



================================ */ 