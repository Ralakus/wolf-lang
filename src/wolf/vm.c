#include "vm.h"

#include "util/memory.h"
#include "util/logger.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* ================================

    Value type

================================ */

void wolf_value_print(wolf_value_t this) {
    switch (this.type) {
        case WOLF_VALUE_TYPE_NIL: 
            wolf_print_raw("nil");
        break;

        case WOLF_VALUE_TYPE_BOOL:
            wolf_print_raw(this.as.boolean ? "true" : "false");
        break;

        case WOLF_VALUE_TYPE_NUMBER:
            wolf_print_raw("%g", this.as.number);
        break;
    }
}

bool wolf_value_is_falsey(wolf_value_t this) {
    return WOLF_VALUE_IS_NIL(this) || (WOLF_VALUE_IS_BOOL(this) && !this.as.boolean);
}

bool wolf_value_is_equal(wolf_value_t a, wolf_value_t b) {
    if(a.type == b.type) {
        switch(a.type) {
            case WOLF_VALUE_TYPE_NIL:
                return true;
            case WOLF_VALUE_TYPE_BOOL: 
                return a.as.boolean == b.as.boolean;
            case WOLF_VALUE_TYPE_NUMBER:
                return a.as.number == b.as.number;
            default:
                return false;
        }
    } else {
        return false;
    }
}

void wolf_value_array_init(wolf_value_array_t* this) {
    this->alloc_len = 0;
    this->len       = 0;
    this->values    = NULL;
}

void wolf_value_array_free(wolf_value_array_t* this) {
    this->values = WOLF_FREE_ARRAY(wolf_value_t, this->values, this->alloc_len);
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
    this-> lines = WOLF_FREE_ARRAY(isize_t, this->lines, this->alloc_len);
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
            this->lines[this->len - 1] = 1;
        }
    }
    ++this->run_len;
}

isize_t wolf_line_array_get(wolf_line_array_t* this, isize_t index) {
    
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
    this->code = WOLF_FREE_ARRAY(uint8_t, this->code, this->alloc_len);
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

static isize_t disassemble_consant_instruction_long(const char* name, wolf_bytecode_t* this, isize_t index) {
    uint16_t constant = *(uint16_t*)(&this->code[index + 1]);
    wolf_print_raw(WOLF_ANSI_GREEN"%-14s "WOLF_ANSI_YELLOW"%4d \'", name, constant);
    wolf_value_print(this->constants.values[constant]);
    wolf_println_raw("\'"WOLF_ANSI_RESET);
    return index + 3;
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

        case WOLF_OP_CONSTANT_L: {
            return disassemble_consant_instruction_long("constantl", this, index);
        } break;

        case WOLF_OP_TRUE: {
            return disassemble_simple_instruction("load true", index);
        } break;

        case WOLF_OP_FALSE: {
            return disassemble_simple_instruction("load false", index);
        } break;

        case WOLF_OP_NIL: {
            return disassemble_simple_instruction("load nil", index);
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
        } break;

        case WOLF_OP_NOT: {
            return disassemble_simple_instruction("not", index);
        } break;

        case WOLF_OP_EQUAL: {
            return disassemble_simple_instruction("equal", index);
        } break;

        case WOLF_OP_GREATER: {
            return disassemble_simple_instruction("greater", index);
        } break;

        case WOLF_OP_LESS: {
            return disassemble_simple_instruction("less", index);
        } break;

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
        wolf_print_raw(WOLF_ANSI_RED"%04d     "WOLF_ANSI_YELLOW, i);
        wolf_value_print(this->constants.values[i]);
        wolf_println_raw(WOLF_ANSI_RESET);
    }

    wolf_noticeln(WOLF_ANSI_CYAN"- Instructions -"WOLF_ANSI_RESET);
    for(isize_t i = 0; i < this->len;) {
        i = wolf_bytecode_disassemble_instruction(this, i);
    }

}

/*
    Serial Format
    Note: wolf_vm_value_t has 4 bytes of padding in between 'type' and 'as'
    HEADER - If any value is zero, it is interpreted as not existing
    32 bit unsigned integer: Index of where constant data starts
    32 bit unsigned integer: Length of constant data
    32 bit unsigned integer: Index of where the bytecode starts
    32 bit unsigned integer: Length of bytecode
    32 bit unsigned integer: Index of where the line data starts
    32 bit unsigned integer: Length of line data
    DATA - Starts 192 bits ( 24 bytes ) from the begining
    CONSTANTS
    BYTECODE
    LINE_DATA
*/


uint8_t* wolf_bytecode_serialize(wolf_bytecode_t* this, size_t* size, bool include_line_data) {

    #define HEADER_OFFSET (sizeof(uint32_t) * 6)

    uint32_t header[6] = {0, 0, 0, 0, 0, 0};

    #define constants_start_index header[0]
    #define constants_len header[1]
    #define bytecode_start_index header[2]
    #define bytecode_len header[3]
    #define line_data_start_index header[4]
    #define line_data_len header[5]

    /* calculates each length */
    constants_len = this->constants.len * sizeof(wolf_value_t);
    bytecode_len  = this->len           * sizeof(uint8_t);
    line_data_len = this->lines.len     * sizeof(isize_t);

    /* calculates header */
    constants_start_index = HEADER_OFFSET;
    bytecode_start_index  = HEADER_OFFSET + constants_len;
    line_data_start_index = HEADER_OFFSET + constants_len + bytecode_len;

    if(!include_line_data) {
        line_data_start_index = 0;
        line_data_len = 0;
    }

    uint32_t total_len = HEADER_OFFSET + constants_len + bytecode_len + line_data_len;

    uint8_t* serialized = NULL;
    isize_t len         = 0;
    serialized          = WOLF_GROW_ARRAY(uint8_t, serialized, 0, total_len);

    memcpy(serialized, header, sizeof(header));
    len += sizeof(header);
    memcpy(serialized + len, this->constants.values, constants_len);
    len += constants_len;
    memcpy(serialized + len, this->code, bytecode_len);
    len += bytecode_len;

    if(include_line_data) {
        memcpy(serialized + len, this->lines.lines, line_data_len);
        len += line_data_len;
    }

    *size = len;

    return serialized;


    #undef constants_start_index
    #undef constants_len 
    #undef bytecode_start_index
    #undef bytecode_len
    #undef line_data_start_index
    #undef line_data_len
    #undef HEADER_OFFSET

}

bool wolf_bytecode_deserialize(wolf_bytecode_t* this, uint8_t* data) {

    wolf_bytecode_free(this);

    uint32_t* header = (uint32_t*)data;

    #define constants_start_index header[0]
    #define constants_len header[1]
    #define bytecode_start_index header[2]
    #define bytecode_len header[3]
    #define line_data_start_index header[4]
    #define line_data_len header[5]


    this->constants.len       = constants_len / sizeof(wolf_value_t);
    this->constants.alloc_len = this->constants.len;
    this->constants.values    = WOLF_GROW_ARRAY(wolf_value_t,
                                                this->constants.values,
                                                0,
                                                this->constants.len);
    memcpy(this->constants.values, data + constants_start_index, constants_len);

    this->len       = bytecode_len / sizeof(uint8_t);
    this->alloc_len = this->len;
    this->code      = WOLF_GROW_ARRAY(uint8_t,
                                      this->code,
                                      0,
                                      this->len);
    memcpy(this->code, data + bytecode_start_index, bytecode_len);

    if(line_data_len != 0 && line_data_start_index != 0) {

        this->lines.len       = line_data_len / sizeof(isize_t);
        this->lines.alloc_len = this->lines.len;
        this->lines.lines     = WOLF_GROW_ARRAY(isize_t,
                                                this->lines.lines,
                                                0,
                                                this->lines.len);
        memcpy(this->lines.lines, data + line_data_start_index, line_data_len);

    }

    return true;

    #undef constants_start_index
    #undef constants_len 
    #undef bytecode_start_index
    #undef bytecode_len
    #undef line_data_start_index
    #undef line_data_len

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

wolf_value_t wolf_vm_peek(wolf_vm_t* this, isize_t distance) {
    return this->stack_top[-1 - distance];
}

wolf_interpret_result_t wolf_vm_run_bytecode(wolf_vm_t* this, wolf_bytecode_t* bytecode) {
    this->bytecode = bytecode;
    this->ip = bytecode->code;

    return run(this);
}

static inline void runtime_error(wolf_vm_t* this, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    wolf_errorln("");
    vfprintf(wolf_error_stream(), fmt, args);
    va_end(args);
    wolf_error_raw("\n");

    size_t instr = this->ip - this->bytecode->code;
    wolf_errorln("[line: %d] in script", wolf_line_array_get(&this->bytecode->lines, instr));

    wolf_vm_reset_stack(this);
}

static wolf_interpret_result_t run(wolf_vm_t* this) {
    #define READ_BYTE() (*this->ip++)
    #define READ_SHORT() (this->ip += 2, (uint16_t)((this->ip[-2] << 8) | this->ip[-1]))
    #define READ_CONSTANT() (this->bytecode->constants.values[READ_BYTE()])
    #define READ_CONSTANT_L() (this->bytecode->constants.values[READ_SHORT()])

    #define BINARY_OP(type, op)                                  \
        ({                                                       \
            if(!WOLF_VALUE_IS_NUMBER(wolf_vm_peek(this, 0))      \
             ||!WOLF_VALUE_IS_NUMBER(wolf_vm_peek(this, 1))) {   \
                runtime_error(this, "Operands must be numbers"); \
                return WOLF_INTERPRET_RUNTIME_ERROR;             \
            }                                                    \
            double b = wolf_vm_pop(this).as.number;              \
            double a = wolf_vm_pop(this).as.number;              \
            wolf_vm_push(this, type(a op b));                    \
        })

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

            case WOLF_OP_CONSTANT_L: {
                wolf_vm_push(this, READ_CONSTANT_L());
            } break;

            case WOLF_OP_TRUE: {
                wolf_vm_push(this, WOLF_VALUE_BOOL(true));
            } break;

            case WOLF_OP_FALSE: {
                wolf_vm_push(this, WOLF_VALUE_BOOL(false));
            } break;

            case WOLF_OP_NIL: {
                wolf_vm_push(this, WOLF_VALUE_NIL);
            } break;

            case WOLF_OP_ADD: {
                BINARY_OP(WOLF_VALUE_NUMBER,+);
            }break;

            case WOLF_OP_SUB: {
                BINARY_OP(WOLF_VALUE_NUMBER,-);
            }break;

            case WOLF_OP_MUL: {
                BINARY_OP(WOLF_VALUE_NUMBER,*);
            }break;

            case WOLF_OP_DIV: {
                BINARY_OP(WOLF_VALUE_NUMBER,/);
            }break;

            case WOLF_OP_NOT: {
                wolf_vm_push(this, WOLF_VALUE_BOOL(wolf_value_is_falsey(wolf_vm_pop(this))));
            } break;

            case WOLF_OP_EQUAL: {
                wolf_value_t b = wolf_vm_pop(this);
                wolf_value_t a = wolf_vm_pop(this);
                wolf_vm_push(this, WOLF_VALUE_BOOL(wolf_value_is_equal(a, b)));
            } break;

            case WOLF_OP_GREATER: {
                BINARY_OP(WOLF_VALUE_BOOL, >);
            } break;

            case WOLF_OP_LESS: {
                BINARY_OP(WOLF_VALUE_BOOL, <);
            } break;

            case WOLF_OP_NEGATE: {
                if(!WOLF_VALUE_IS_NUMBER(wolf_vm_peek(this, 0))) {
                    runtime_error(this, "Operand must be a number");
                    return WOLF_INTERPRET_RUNTIME_ERROR;
                }
                wolf_vm_push(this, WOLF_VALUE_NUMBER(-wolf_vm_pop(this).as.number));
            } break;

            case WOLF_OP_RETURN: {
                wolf_success("[return]: "WOLF_ANSI_YELLOW);
                wolf_value_print(wolf_vm_pop(this));
                wolf_print_raw("\n"WOLF_ANSI_RESET);
                return WOLF_INTERPRET_OK;
            } break;

            default: {
                runtime_error(this, "Unknown instruction: %d", instruction);
                return WOLF_INTERPRET_RUNTIME_ERROR;
            } break;
        }
    }

    #undef BINARY_OP
    #undef READ_CONSTANT_L
    #undef READ_CONSTANT
    #undef READ_SHORT
    #undef READ_BYTE
}