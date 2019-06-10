#pragma once

#include "common.h"

/* ================================

    Wolf strings

    Wolf String format:
    [isize_t alloc_len][isize_t len][user data][null term]
     ^ Hidden header                 ^ User pointer

================================ */

typedef struct {
    isize_t alloc_len;
    isize_t len;
    char    buf[];
} wolf_string_header_t;

typedef char*             wolf_str_t;
typedef const char* const_wolf_str_t;

/* makes from c string */
wolf_str_t wolf_str_make(const char* cstr, isize_t len);
/* frees wolf string, do not use free() */
void wolf_str_free(wolf_str_t wstr);

/* copies wolf string into new wolf string */
wolf_str_t wolf_str_copy(const_wolf_str_t wstr);

/* gets length of wolf string */
isize_t wolf_str_len(const_wolf_str_t wstr);

/* makes new string by concatenating two strings  */
wolf_str_t wolf_str_concat_new(const_wolf_str_t wstr1, const_wolf_str_t wstr2);

/* concatenates second string onto first string */
wolf_str_t wolf_str_concat(wolf_str_t wstr1, const_wolf_str_t wstr2);

/* compares two strings */
bool wolf_str_equal(const_wolf_str_t wstr1, const_wolf_str_t wstr2);