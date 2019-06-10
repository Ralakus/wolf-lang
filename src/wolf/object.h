#pragma once

#include "vm.h"
#include "util/string.h"

typedef enum {
    WOLF_OBJECT_TYPE_STRING,
} wolf_object_type_t;

struct wolf_object {
    wolf_object_type_t type;
    struct wolf_object* next;
};

typedef struct {
    wolf_object_t        object;
    wolf_string_header_t str;
} wolf_object_string_t;

#define WOLF_OBJECT_TYPE(value) ((value).as.object->type)

/* check if object is type */
bool wolf_object_is_type(wolf_value_t value, wolf_object_type_t type);

#define WOLF_OBJECT_IS_STRING(value) (wolf_object_is_type(value, WOLF_OBJECT_TYPE_STRING))

#define WOLF_OBJECT_AS_STRING(value)  ((wolf_object_string_t*)(value).as.object)
#define WOLF_OBJECT_AS_CSTRING(value) (WOLF_OBJECT_AS_STRING(value)->str.buf)

/* makes string object from c string */
wolf_object_string_t* wolf_object_string_copy(wolf_bytecode_t* bytecode, const char* chars, isize_t len);

/* concacts two string objects and returns a new string */
wolf_object_string_t* wolf_object_string_concat(wolf_bytecode_t* bytecode, const wolf_object_string_t* str1, const wolf_object_string_t* str2);

/* prints object's value */
void wolf_object_print(wolf_value_t value);

/* frees all objects in bytecode */
void wolf_objects_free(wolf_bytecode_t* bytecode);

