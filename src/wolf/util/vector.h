#pragma once

#include "memory.h"

#define WOLF_VEC_NAME_NO_T(type) wolf_vec_ ## type
#define WOLF_VEC_NAME(type) WOLF_VEC_NAME_NO_T(type ## _t)
#define WOLF_VEC_NAME_DEF(type, def) WOLF_VEC_NAME_NO_T(type ## _ ## def)

#define WOLF_VEC_DEFINE(type)                                      \
    typedef struct WOLF_VEC_NAME_NO_T(type) {                      \
        isize_t alloc_len;                                         \
        isize_t len;                                               \
        type    data[];                                            \
    } WOLF_VEC_NAME(type);                                         \
                                                                   \
    WOLF_VEC_NAME(type)* WOLF_VEC_NAME_DEF(type, make)             \
    (isize_t size);                                                \
                                                                   \
    WOLF_VEC_NAME(type)* WOLF_VEC_NAME_DEF(type, free)             \
    (WOLF_VEC_NAME(type)* vec);                                    \
                                                                   \
    type* WOLF_VEC_NAME_DEF(type, at)                              \
    (WOLF_VEC_NAME(type)* vec, isize_t index);                     \
                                                                   \
    type* WOLF_VEC_NAME_DEF(type, push_back_arr)                   \
    (WOLF_VEC_NAME(type)* vec, type* data, isize_t len);



#define WOLF_VEC_IMPLEMENT(type)                                   \
    WOLF_VEC_NAME(type)* WOLF_VEC_NAME_DEF(type, make)             \
    (isize_t size) {                                               \
        return wolf_realloc(                                       \
            NULL,                                                  \
            0,                                                     \
            sizeof(WOLF_VEC_NAME(type)) + (size * sizeof(type)));  \
    }                                                              \
                                                                   \
    WOLF_VEC_NAME(type)* WOLF_VEC_NAME_DEF(type, free)             \
    (WOLF_VEC_NAME(type)* vec) {                                   \
        return WOLF_FREE(WOLF_VEC_NAME(type), vec);                \
    }                                                              \
                                                                   \
    type* WOLF_VEC_NAME_DEF(type, at)                              \
    (WOLF_VEC_NAME(type)* vec, isize_t index) {                    \
        return vec->data +index;                                   \
    }                                                              \
                                                                   \
    type* WOLF_VEC_NAME_DEF(type, push_back_arr)                   \
    (WOLF_VEC_NAME(type)* vec, type* data, isize_t len) {          \
    }


#define WOLF_VEC_DEFINE_AND_IMPLEMENT(type) WOLF_VEC_DEFINE(type) WOLF_VEC_IMPLEMENT(type)