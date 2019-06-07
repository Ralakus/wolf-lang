#pragma once

#include "common.h"

/* main memory function, can allocate, reallocate, and free pointers */
void* wolf_realloc(void* ptr, size_t old_size, size_t new_size);

#define min(a,b) \
    ({  __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a < _b ? _a : _b; })

#define max(a,b) \
    ({  __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })

#define WOLF_GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define WOLF_GROW_ARRAY(type, ptr, old_len, len)   \
    ((type*)wolf_realloc((ptr),                    \
                         (old_len) * sizeof(type), \
                         (len)     * sizeof(type)))

#define WOLF_FREE_ARRAY(type, ptr, old_len)        \
    ((type*)wolf_realloc((ptr),                    \
                         (old_len) * sizeof(type), \
                         0))

#define WOLF_ALLOCATE(type, count) \
    ((type*)wolf_realloc(NULL, 0, sizeof(type) * (count)))

#define WOLF_FREE(type, ptr) \
    ((type*)wolf_realloc(ptr, sizeof(type), 0))
