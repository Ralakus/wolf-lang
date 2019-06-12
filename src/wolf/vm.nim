import util/common

{.push header: "\"wolf/vm.h\"".}

##  ================================
##
##     Value type
##
## ================================

type
  wolf_object = object

type
  INNER_C_UNION_vm_23* {.bycopy, union.} = object
    boolean*: bool
    number*: cdouble
    `object`*: ptr wolf_object_t

  wolf_object_t* = wolf_object
  wolf_value_type_t* = enum
    WOLF_VALUE_TYPE_NIL, WOLF_VALUE_TYPE_BOOL, WOLF_VALUE_TYPE_NUMBER,
    WOLF_VALUE_TYPE_OBJECT
  wolf_value_t* {.bycopy.} = object
    `type`*: wolf_value_type_t
    `as`*: INNER_C_UNION_vm_23



##  prints value to print stream
proc wolf_value_print*(value: wolf_value_t)

##  if value would be a boolean false
proc wolf_value_is_falsey*(value: wolf_value_t): bool


##  compares if two values are equal
proc wolf_value_is_equal*(a: wolf_value_t; b: wolf_value_t): bool
type
  wolf_value_array_t* {.bycopy.} = object
    alloc_len*: isize
    len*: isize
    values*: ptr wolf_value_t


##  inits value array, must be done before use
proc wolf_value_array_init*(array: ptr wolf_value_array_t)

##  frees value array, must be done after use
proc wolf_value_array_free*(array: ptr wolf_value_array_t)

##  writes a single value to the value array
proc wolf_value_array_write*(array: ptr wolf_value_array_t; value: wolf_value_t)

##  ================================
##
##     Run length encoded line container
##
## ================================

type
  wolf_line_array_t* {.bycopy.} = object
    alloc_len*: isize
    len*: isize
    run_len*: isize
    lines*: ptr isize         ##  Value, Length


##  inits line array, must be done before use
proc wolf_line_array_init*(array: ptr wolf_line_array_t)

##  frees line array, must be done after use
proc wolf_line_array_free*(array: ptr wolf_line_array_t)

##  writes a single line to line array
proc wolf_line_array_write*(array: ptr wolf_line_array_t; line: isize)

##  gets a single line from array at index
proc wolf_line_array_get*(array: ptr wolf_line_array_t; index: isize): isize

##  ================================
##
##     Bytecode
##
## ================================

type
  wolf_opcode_t* = enum
    WOLF_OP_CONSTANT, WOLF_OP_CONSTANT_L, WOLF_OP_TRUE, WOLF_OP_FALSE, WOLF_OP_NIL,
    WOLF_OP_ADD, WOLF_OP_SUB, WOLF_OP_MUL, WOLF_OP_DIV, WOLF_OP_NOT, WOLF_OP_EQUAL,
    WOLF_OP_GREATER, WOLF_OP_LESS, WOLF_OP_NEGATE, WOLF_OP_RETURN
  wolf_bytecode_t* {.bycopy.} = object
    constants*: wolf_value_array_t
    alloc_len*: isize
    len*: isize
    code*: ptr system.uint8
    lines*: wolf_line_array_t
    objects*: ptr wolf_object_t



##  inits bytecode, must be done before use
proc wolf_bytecode_init*(bytecode: ptr wolf_bytecode_t)

##  frees bytecode, must be done after use
proc wolf_bytecode_free*(bytecode: ptr wolf_bytecode_t)

##  writes a single byte to bytecode
proc wolf_bytecode_write*(bytecode: ptr wolf_bytecode_t; byte: system.uint8; line: isize)

##  writes a constant to bytecode
proc wolf_bytecode_write_constant*(bytecode: ptr wolf_bytecode_t;
                                  value: wolf_value_t): isize

##  disassembles a single instruction in bytecode at given index
proc wolf_bytecode_disassemble_instruction*(bytecode: ptr wolf_bytecode_t;
    index: isize): isize

##  disassembles bytecode and prints it out
proc wolf_bytecode_disassemble*(bytecode: ptr wolf_bytecode_t; name: cstring)

##
##     Serial Format
##     Note: wolf_vm_value_t has 4 bytes of padding in between 'type' and 'as'
##     HEADER - If any value is zero, it is interpreted as not existing
##     32 bit unsigned integer: Index of where constant data starts
##     32 bit unsigned integer: Length of constant data
##     32 bit unsigned integer: Index of where the bytecode starts
##     32 bit unsigned integer: Length of bytecode
##     32 bit unsigned integer: Index of where the line data starts
##     32 bit unsigned integer: Length of line data
##     DATA - Starts 192 bits ( 24 bytes ) from the begining
##     CONSTANTS
##     BYTECODE
##     LINE_DATA
##

##  serializes bytecode and returns bytecode in serial format
proc wolf_bytecode_serialize*(bytecode: ptr wolf_bytecode_t; size: ptr csize;
                             include_line_data: bool): ptr system.uint8

##  deserializes bytecode from serial format and returns bytecode
proc wolf_bytecode_deserialize*(bytecode: ptr wolf_bytecode_t; data: ptr system.uint8): bool

##  ================================
##
##     Virtual Machine
##
## ================================

type
  wolf_vm_t* {.bycopy.} = object
    debug_trace*: bool
    bytecode*: ptr wolf_bytecode_t
    ip*: ptr system.uint8
    stack*: array[256, wolf_value_t]
    stack_top*: ptr wolf_value_t
    objects*: ptr wolf_object_t

  wolf_interpret_result_t* = enum
    WOLF_INTERPRET_OK, WOLF_INTERPRET_COMPILE_ERROR, WOLF_INTERPRET_RUNTIME_ERROR


##  inits vm, must be done before use
proc wolf_vm_init*(vm: ptr wolf_vm_t)

##  frees vm, must be done after use
proc wolf_vm_free*(vm: ptr wolf_vm_t)

##  enables or disables vm execution tracing
proc wolf_vm_set_debug_trace*(vm: ptr wolf_vm_t; value: bool)

##  clears vm stack
proc wolf_vm_reset_stack*(vm: ptr wolf_vm_t)

##  pushes value to vm stack
proc wolf_vm_push*(vm: ptr wolf_vm_t; value: wolf_value_t)

##  pops value from vm stack
proc wolf_vm_pop*(vm: ptr wolf_vm_t): wolf_value_t

##  gets value from the stack without modifying the stack
proc wolf_vm_peek*(vm: ptr wolf_vm_t; distance: isize): wolf_value_t

##  runs bytecode on vm
proc wolf_vm_run_bytecode*(vm: ptr wolf_vm_t; bytecode: ptr wolf_bytecode_t): wolf_interpret_result_t

{.pop.}