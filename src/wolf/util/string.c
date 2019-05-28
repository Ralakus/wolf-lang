#include "string.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>

#define STR_TO_HEADER(wstr) ((wolf_string_header_t*)&wstr[-sizeof(wolf_string_header_t)])

char* wolf_str_make(const char* cstr, isize_t len) {
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
    free(strhdr);
}

isize_t wolf_str_len(wolf_str_t wstr) {
    return STR_TO_HEADER(wstr)->len;
}

char* wolf_str_concat(const wolf_str_t wstr1, const wolf_str_t wstr2) {

}