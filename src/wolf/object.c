#include "object.h"
#include "vm.h"

#include "util/memory.h"
#include "util/logger.h"

#include <string.h>

bool wolf_object_is_type(wolf_value_t value, wolf_object_type_t type) {
    return WOLF_VALUE_IS_OBJECT(value) && WOLF_OBJECT_TYPE(value) == type;
}

#define ALLOCATE_OBJECT(type, object_type) \
    ((type*)allocate_object(sizeof(type), object_type))

static inline wolf_object_t* allocate_object(size_t size, wolf_object_type_t type) {
    wolf_object_t* object = (wolf_object_t*)wolf_realloc(NULL, 0, size);
    object->type = type;
    return object;
}

static inline wolf_object_string_t* allocate_string(char* chars, isize_t len) {
    wolf_object_string_t* string = ALLOCATE_OBJECT(wolf_object_string_t, WOLF_OBJECT_TYPE_STRING);
    string->len = len;
    string->str = chars;

    return string;
}


wolf_object_string_t* wolf_object_string_copy(const char* chars, isize_t len) {
    char* heap_chars = WOLF_ALLOCATE(char, len + 1);
    memcpy(heap_chars, chars, len);
    heap_chars[len] = '\0';

    return allocate_string(heap_chars, len);
}

wolf_object_string_t* wolf_object_string_take(char* chars, isize_t len) {
    return allocate_string(chars, len);
}

void wolf_object_print(wolf_value_t value) {
    switch(WOLF_OBJECT_TYPE(value)) {
        case WOLF_OBJECT_TYPE_STRING:
            wolf_print_raw("%s", WOLF_OBJECT_AS_CSTRING(value));
            break;
    }
}