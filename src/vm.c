#include "vm.h"

#include "util/memory.h"

/* ================================

    Bytecode

================================ */ 

void wolf_bytecode_init(wolf_bytecode_t* this) {
    this->alloc_len = 0;
    this->len      = 0;
    this->code     = NULL;
}

void wolf_bytecode_free(wolf_bytecode_t* this) {
    WOLF_FREE_ARRAY(uint8_t, this->code, this->alloc_len);
    wolf_bytecode_init(this);
}

void wolf_bytecode_write(wolf_bytecode_t* this, uint8_t byte) {
    if(this->alloc_len < this->len + 1) {
        ptrdiff_t old_alloc_len = this->alloc_len;
        this->alloc_len = WOLF_GROW_CAPACITY(this->alloc_len);
        this->code = WOLF_GROW_ARRAY(uint8_t, 
                     this->code,
                     old_alloc_len,
                     this->alloc_len);
    }

    this->code[this->len] = byte;
    ++this->len;
}