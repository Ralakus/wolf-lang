#include "vm.h"

#include "util/memory.h"
#include "util/logger.h"

/* ================================

    Value type

================================ */

void wolf_value_print(wolf_value_t this) {
    wolf_print_raw("%g", this);
}

void wolf_value_array_init(wolf_value_array_t* this) {
    this->alloc_len = 0;
    this->len       = 0;
    this->values    = NULL;
}

void wolf_value_array_free(wolf_value_array_t* this) {
    WOLF_FREE_ARRAY(wolf_value_t, this->values, this->alloc_len);
    wolf_value_array_init(this);
}

void wolf_value_array_write(wolf_value_array_t* this, wolf_value_t value) {
    if(this->alloc_len < this->len + 1) {
        ptrdiff_t old_alloc_len = this->alloc_len;
        this->alloc_len = WOLF_GROW_CAPACITY(this->alloc_len);
        this->values = WOLF_GROW_ARRAY(wolf_value_t, 
                       this->values,
                       old_alloc_len,
                       this->alloc_len);
    }

    this->values[this->len] = value;
    ++this->len;
}

/* ================================

    Run length encoded line container

================================ */ 

void wolf_line_array_init(wolf_line_array_t* this) {
    this->alloc_len = 0;
    this->len       = 0;
    this->run_len   = 0;
    this->lines     = NULL;
}

void wolf_line_array_free(wolf_line_array_t* this) {
    WOLF_FREE_ARRAY(isize_t, this->lines, this->alloc_len);
    wolf_line_array_init(this);
}

void wolf_line_array_write(wolf_line_array_t* this, isize_t line) {
    if(this->alloc_len < this->len + 2) {
        ptrdiff_t old_alloc_len = this->alloc_len;
        this->alloc_len = WOLF_GROW_CAPACITY(this->alloc_len);
        this->lines = WOLF_GROW_ARRAY(isize_t, 
                      this->lines,
                      old_alloc_len,
                      this->alloc_len);
    }

    if(this->run_len == 0) {
        this->lines[0] = line;
        this->lines[1] = 1;
        this->len = 2;
    } else {
        if(line == this->lines[this->len - 2]) {
            ++this->lines[this->len - 1];
        } else {
            this->len += 2;
            this->lines[this->len - 2] = line;
            this->lines[this->len - 1]     = 1;
        }
    }
    ++this->run_len;
}

isize_t wolf_line_array_get  (wolf_line_array_t* this, isize_t index) {
    
    isize_t counter = 0;
    for(isize_t i = 0; i < this->len; i += 2) {
        if(this->lines[i + 1] + counter >= index) {
            return this->lines[i];
        }
    }

    return 0;
}

/* ================================

    Bytecode

================================ */ 

void wolf_bytecode_init(wolf_bytecode_t* this) {
    this->alloc_len = 0;
    this->len       = 0;
    this->code      = NULL;
    wolf_value_array_init(&this->constants);
    wolf_line_array_init(&this->lines);
}

void wolf_bytecode_free(wolf_bytecode_t* this) {
    WOLF_FREE_ARRAY(uint8_t, this->code, this->alloc_len);
    wolf_value_array_free(&this->constants);
    wolf_line_array_free(&this->lines);
    wolf_bytecode_init(this);
}

void wolf_bytecode_write(wolf_bytecode_t* this, uint8_t byte, isize_t line) {
    if(this->alloc_len < this->len + 1) {
        ptrdiff_t old_alloc_len = this->alloc_len;
        this->alloc_len = WOLF_GROW_CAPACITY(this->alloc_len);
        this->code = WOLF_GROW_ARRAY(uint8_t, 
                     this->code,
                     old_alloc_len,
                     this->alloc_len);
    }

    this->code [this->len] = byte;
    wolf_line_array_write(&this->lines, line);
    ++this->len;
}

isize_t wolf_bytecode_write_constant(wolf_bytecode_t* this, wolf_value_t value) {
    wolf_value_array_write(&this->constants, value);
    return this->constants.len - 1;
}

static isize_t disassemble_simple_instruction(const char* name, isize_t index) {
    wolf_println_raw(WOLF_ANSI_GREEN"%-16s"WOLF_ANSI_RESET, name);
    return index + 1;
}

static isize_t disassemble_consant_instruction(const char* name, wolf_bytecode_t* this, isize_t index) {
    uint8_t constant = this->code[index + 1];
    wolf_print_raw(WOLF_ANSI_GREEN"%-14s "WOLF_ANSI_YELLOW"%4d \'", name, constant);
    wolf_value_print(this->constants.values[constant]);
    wolf_println_raw("\'"WOLF_ANSI_RESET);
    return index + 2;
}

isize_t wolf_bytecode_disassemble_instruction(wolf_bytecode_t* this, isize_t index) {
    wolf_print_raw(WOLF_ANSI_RED"%04d", index);

    if(index > 0) {
        if(wolf_line_array_get(&this->lines, index) == wolf_line_array_get(&this->lines, index - 1)) wolf_print_raw("   | "WOLF_ANSI_RESET);
        else wolf_print_raw("%4d "WOLF_ANSI_RESET, wolf_line_array_get(&this->lines, index));
    }   else wolf_print_raw("%4d "WOLF_ANSI_RESET, wolf_line_array_get(&this->lines, index));

    uint8_t instruction = this->code[index];
    switch (instruction) {

        case WOLF_OP_CONSTANT: {
            return disassemble_consant_instruction("constant", this, index);
        } break;

        case WOLF_OP_ADD: {
            return disassemble_simple_instruction("add", index);
        } break;

        case WOLF_OP_SUB: {
            return disassemble_simple_instruction("subtract", index);
        } break;

        case WOLF_OP_MUL: {
            return disassemble_simple_instruction("multiply", index);
        } break;

        case WOLF_OP_DIV: {
            return disassemble_simple_instruction("divide", index);
        }

        case WOLF_OP_NEGATE: {
            return disassemble_simple_instruction("negate", index);
        } break;

        case WOLF_OP_RETURN: {
            return disassemble_simple_instruction("return", index);
        } break;

        default: {
            wolf_println_raw(WOLF_ANSI_RED"Unknown opcode %d"WOLF_ANSI_RESET, instruction);
            return index + 1;
        } break;
        
    }
}

void wolf_bytecode_disassemble(wolf_bytecode_t* this, const char* name) {
    wolf_noticeln(WOLF_ANSI_CYAN"--== %s ==--"WOLF_ANSI_RESET, name);

    wolf_noticeln(WOLF_ANSI_CYAN"- Constants -"WOLF_ANSI_RESET);
    for(isize_t i = 0; i < this->constants.len; i++) {
        wolf_print(WOLF_ANSI_RED"%04d      "WOLF_ANSI_YELLOW, i);
        wolf_value_print(this->constants.values[i]);
        wolf_println(WOLF_ANSI_RESET);
    }

    wolf_noticeln(WOLF_ANSI_CYAN"- Instructions -"WOLF_ANSI_RESET);
    for(isize_t i = 0; i < this->len;) {
        i = wolf_bytecode_disassemble_instruction(this, i);
    }

}

/* ================================

    Virtual Machine

================================ */ 

static wolf_interpret_result_t run(wolf_vm_t* this);

void wolf_vm_init(wolf_vm_t* this) {
    this->bytecode = NULL;
    this->stack_top = this->stack;
}

void wolf_vm_free(wolf_vm_t* this) {
    wolf_vm_init(this);
}

void wolf_vm_set_debug_trace(wolf_vm_t* this, bool value) {
    this->debug_trace = value;
}

void wolf_vm_reset_stack(wolf_vm_t* this) {
    this->stack_top = this->stack;
}

void wolf_vm_push(wolf_vm_t* this, wolf_value_t value) {
    *this->stack_top = value;
    ++this->stack_top;
}

wolf_value_t wolf_vm_pop(wolf_vm_t* this) {
    --this->stack_top;
    return *this->stack_top;
}

wolf_interpret_result_t wolf_vm_run_bytecode(wolf_vm_t* this, wolf_bytecode_t* bytecode) {
    this->bytecode = bytecode;
    this->ip = bytecode->code;

    return run(this);
}

static wolf_interpret_result_t run(wolf_vm_t* this) {
    #define READ_BYTE() (*this->ip++)
    #define READ_CONSTANT() (this->bytecode->constants.values[READ_BYTE()])

    #define BINARY_OP(op)                 \
        do {                              \
            double b = wolf_vm_pop(this); \
            double a = wolf_vm_pop(this); \
            wolf_vm_push(this, a op b);   \
        } while(false)

    for(;;) {

        if(this->debug_trace) {
            wolf_print("[stack]:  ");
            for(wolf_value_t* slot = this->stack; slot < this->stack_top; slot++) {
                wolf_print_raw(WOLF_ANSI_YELLOW"[ ");
                wolf_value_print(*slot);
                wolf_print_raw(" ]"WOLF_ANSI_RESET);
            }
            wolf_print_raw("\n");
            wolf_bytecode_disassemble_instruction(this->bytecode, this->ip - this->bytecode->code);
        }

        uint8_t instruction;

        switch(instruction = READ_BYTE()) {

            case WOLF_OP_CONSTANT: {
                wolf_vm_push(this, READ_CONSTANT());
            } break;

            case WOLF_OP_ADD: BINARY_OP(+); break;
            case WOLF_OP_SUB: BINARY_OP(-); break;
            case WOLF_OP_MUL: BINARY_OP(*); break;
            case WOLF_OP_DIV: BINARY_OP(/); break;

            case WOLF_OP_NEGATE: {
                wolf_vm_push(this, -wolf_vm_pop(this));
            } break;

            case WOLF_OP_RETURN: {
                wolf_success("[return]: "WOLF_ANSI_YELLOW);
                wolf_value_print(wolf_vm_pop(this));
                wolf_print_raw("\n"WOLF_ANSI_RESET);
                return WOLF_INTERPRET_OK;
            } break;

            default: {
                return WOLF_INTERPRET_RUNTIME_ERROR;
            } break;
        }
    }

    #undef BINARY_OP
    #undef READ_CONSTANT
    #undef READ_BYTE
}