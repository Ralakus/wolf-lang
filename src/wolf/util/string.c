#include "string.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>

#define STR_TO_HEADER(wstr) ((wolf_string_header_t*)&wstr[-sizeof(wolf_string_header_t)])

wolf_str_t wolf_str_make(const char* cstr, isize_t len) {
    wolf_string_header_t* strhdr =
        (wolf_string_header_t*)wolf_realloc(NULL, 0, sizeof(wolf_string_header_t) + len + 1);
    if(strhdr == NULL) {
        return NULL;
    }

    if(memcpy(strhdr->buf, cstr, len) == NULL) {
        free(strhdr);
        return NULL;
    }

    strhdr->buf[len]  = '\0';
    strhdr->alloc_len = len;
    strhdr->len       = len;

    return strhdr->buf;
}

void wolf_str_free(wolf_str_t wstr) {
    wolf_string_header_t* strhdr = STR_TO_HEADER(wstr);
    WOLF_FREE(wolf_string_header_t, strhdr);
}

isize_t wolf_str_len(const_wolf_str_t wstr) {
    return STR_TO_HEADER(wstr)->len;
}

wolf_str_t wolf_str_concat_new(const_wolf_str_t wstr1, const_wolf_str_t wstr2) {
    isize_t wstr1len = STR_TO_HEADER(wstr1)->len;
    isize_t wstr2len = STR_TO_HEADER(wstr2)->len;
    isize_t len = wstr1len + wstr2len;

    wolf_string_header_t* strhdr =
        (wolf_string_header_t*)wolf_realloc(NULL, 0, sizeof(wolf_string_header_t) + len + 1);
    if(strhdr == NULL) {
        return NULL;
    }

    if(memcpy(strhdr->buf, wstr1, wstr1len) == NULL) {
        WOLF_FREE(wolf_string_header_t, strhdr);
        return NULL;
    }
    if(memcpy(strhdr->buf + wstr1len, wstr2, wstr2len) == NULL) {
        WOLF_FREE(wolf_string_header_t, strhdr);
        return NULL;
    }

    strhdr->buf[len]  = '\0';
    strhdr->len       = len;
    strhdr->alloc_len = len;

    return strhdr->buf;
}

wolf_str_t wolf_str_concat(wolf_str_t wstr1, const_wolf_str_t wstr2) {
    isize_t wstr1len = STR_TO_HEADER(wstr1)->len;
    isize_t wstr2len = STR_TO_HEADER(wstr2)->len;
    isize_t len = wstr1len + wstr2len;

    wolf_string_header_t* strhdr =
        (wolf_string_header_t*)wolf_realloc(
            STR_TO_HEADER(wstr1),
            sizeof(wolf_string_header_t) + wstr1len + 1,
            sizeof(wolf_string_header_t) + len + 1);
    if(strhdr == NULL) {
        return NULL;
    }

    if(memcpy(strhdr->buf + wstr1len, wstr2, wstr2len) == NULL) {
        WOLF_FREE(wolf_string_header_t, strhdr);
        return NULL;
    }

    strhdr->buf[len]  = '\0';
    strhdr->len       = len;
    strhdr->alloc_len = len;

    return strhdr->buf;
}

bool wolf_str_equal(const_wolf_str_t wstr1, const_wolf_str_t wstr2) {
    return (STR_TO_HEADER(wstr1)->len == STR_TO_HEADER(wstr2)->len) &&
           (memcmp(wstr1, wstr2, STR_TO_HEADER(wstr1)->len) == 0);
}