#include "object.h"
#include "vm.h"

#include "util/memory.h"
#include "util/logger.h"

#include <string.h>

bool wolf_object_is_type(wolf_value_t value, wolf_object_type_t type) {
    return WOLF_VALUE_IS_OBJECT(value) && WOLF_OBJECT_TYPE(value) == type;
}

#define ALLOCATE_OBJECT(bytecode, type, object_type) \
    ((type*)allocate_object(bytecode, sizeof(type), object_type))

static inline wolf_object_t* allocate_object(wolf_bytecode_t* bytecode, size_t size, wolf_object_type_t type) {
    wolf_object_t* object = (wolf_object_t*)wolf_realloc(NULL, 0, size);
    object->type = type;

    object->next = bytecode->objects;
    bytecode->objects = object;
    return object;
}

static inline wolf_object_string_t* allocate_string(wolf_bytecode_t* bytecode, char* chars, isize_t len) {
    wolf_object_string_t* string = ALLOCATE_OBJECT(bytecode, wolf_object_string_t, WOLF_OBJECT_TYPE_STRING);
    string->len = len;
    string->str = chars;

    return string;
}


wolf_object_string_t* wolf_object_string_copy(wolf_bytecode_t* bytecode, const char* chars, isize_t len) {
    char* heap_chars = WOLF_ALLOCATE(char, len + 1);
    memcpy(heap_chars, chars, len);
    heap_chars[len] = '\0';

    return allocate_string(bytecode, heap_chars, len);
}

wolf_object_string_t* wolf_object_string_take(wolf_bytecode_t* bytecode, char* chars, isize_t len) {
    return allocate_string(bytecode, chars, len);
}

void wolf_object_print(wolf_value_t value) {
    switch(WOLF_OBJECT_TYPE(value)) {
        case WOLF_OBJECT_TYPE_STRING:
            wolf_print_raw("%s", WOLF_OBJECT_AS_CSTRING(value));
            break;
    }
}

static void wolf_object_free(wolf_object_t* object) {
    switch(object->type) {
        case WOLF_OBJECT_TYPE_STRING: {
            wolf_object_string_t* str = (wolf_object_string_t*)object;
            WOLF_FREE_ARRAY(char, str->str, str->len + 1);
            WOLF_FREE(wolf_object_string_t, object);
            break;
        }
    }
}

void wolf_objects_free(wolf_bytecode_t* bytecode) {
    wolf_object_t* obj = bytecode->objects;
    while(obj != NULL) {
        wolf_object_t* next = obj->next;
        wolf_object_free(obj);
        obj = next;
    }
}