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

typedef char* wolf_str_t;

/* makes from c string */
char* wolf_str_make(const char* cstr, isize_t len);
/* frees wolf string, do not use free() */
void wolf_str_free(wolf_str_t wstr);

/* copies wolf string into new wolf string */
char* wolf_str_copy(const wolf_str_t wstr);

/* gets length of wolf string */
isize_t wolf_str_len(wolf_str_t wstr);

/* makes new string by concatenating two strings  */
char* wolf_str_concat(const wolf_str_t wstr1, const wolf_str_t wstr2);