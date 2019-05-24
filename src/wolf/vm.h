#pragma once

#include "util/common.h"

/* ================================

    Value type

================================ */ 

typedef double wolf_value_t;

void wolf_value_print(wolf_value_t value);

typedef struct {
    isize_t       alloc_len;
    isize_t       len;
    wolf_value_t* values;
} wolf_value_array_t;

/* inits value array, must be done before use */
void wolf_value_array_init(wolf_value_array_t* array);
/* frees value array, must be done after use */
void wolf_value_array_free(wolf_value_array_t* array);

/* writes a single value to the value array */
void wolf_value_array_write(wolf_value_array_t* array, wolf_value_t value);

/* ================================

    Run length encoded line container

================================ */ 

typedef struct {
    isize_t  alloc_len;
    isize_t  len;
    isize_t  run_len;
    isize_t* lines; /* Value, Length */
} wolf_line_array_t;

/* inits line array, must be done before use*/
void wolf_line_array_init(wolf_line_array_t* array);
/* frees line array, must be done after use */
void wolf_line_array_free(wolf_line_array_t* array);

/* writes a single line to line array */
void wolf_line_array_write(wolf_line_array_t* array, isize_t line);
/* gets a single line from array at index */
isize_t wolf_line_array_get(wolf_line_array_t* array, isize_t index);

/* ================================

    Bytecode

================================ */ 

typedef enum {
    WOLF_OP_CONSTANT,
    WOLF_OP_ADD,
    WOLF_OP_SUB,
    WOLF_OP_MUL,
    WOLF_OP_DIV,
    WOLF_OP_NEGATE,
    WOLF_OP_RETURN,
} wolf_opcode_t;

typedef struct {
    wolf_value_array_t constants;
    isize_t            alloc_len;
    isize_t            len;
    uint8_t*           code;
    wolf_line_array_t  lines;
} wolf_bytecode_t;

/* inits bytecode, must be done before use */
void wolf_bytecode_init(wolf_bytecode_t* bytecode);
/* frees bytecode, must be done after use */
void wolf_bytecode_free(wolf_bytecode_t* bytecode);

/* writes a single byte to bytecode */
void wolf_bytecode_write(wolf_bytecode_t* bytecode, uint8_t byte, isize_t line);

/* writes a constant to bytecode */
isize_t wolf_bytecode_write_constant(wolf_bytecode_t* bytecode, wolf_value_t value);

/* disassembles a single instruction in bytecode at given index */
isize_t wolf_bytecode_disassemble_instruction(wolf_bytecode_t* bytecode, isize_t index);
/* disassembles bytecode and prints it out */
void wolf_bytecode_disassemble(wolf_bytecode_t* bytecode, const char* name);

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

/* serializes bytecode and returns bytecode in serial format */
uint8_t* wolf_bytecode_serialize(wolf_bytecode_t* bytecode, size_t* size, bool include_line_data);
/* deserializes bytecode from serial format and returns bytecode */
bool wolf_bytecode_deserialize(wolf_bytecode_t* bytecode, uint8_t* data);

/* ================================

    Virtual Machine

================================ */ 

#ifndef WOLF_VM_STACK_SIZE
    #define WOLF_VM_STACK_SIZE 256
#endif

typedef struct {
    bool             debug_trace;
    wolf_bytecode_t* bytecode;
    uint8_t*         ip;
    wolf_value_t     stack[WOLF_VM_STACK_SIZE];
    wolf_value_t*    stack_top;
} wolf_vm_t;

typedef enum {
    WOLF_INTERPRET_OK,
    WOLF_INTERPRET_COMPILE_ERROR,
    WOLF_INTERPRET_RUNTIME_ERROR,
} wolf_interpret_result_t;

/* inits vm, must be done before use */
void wolf_vm_init(wolf_vm_t* vm);
/* frees vm, must be done after use */
void wolf_vm_free(wolf_vm_t* vm);

/* enables or disables vm execution tracing */
void wolf_vm_set_debug_trace(wolf_vm_t* vm, bool value);

/* clears vm stack */
void wolf_vm_reset_stack(wolf_vm_t* vm);
/* pushes value to vm stack */
void wolf_vm_push(wolf_vm_t* vm, wolf_value_t value);
/* pops value from vm stack */
wolf_value_t wolf_vm_pop(wolf_vm_t* vm);

/* runs bytecode on vm */
wolf_interpret_result_t wolf_vm_run_bytecode(wolf_vm_t* vm, wolf_bytecode_t* bytecode);