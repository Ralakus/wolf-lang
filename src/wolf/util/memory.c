#include <stdlib.h>
#include <string.h>

#include "memory.h"

void* wolf_realloc(void* ptr, size_t old_size, size_t new_size) {
    if(new_size == 0) {
        free(ptr);
        return NULL;
    }

    return realloc(ptr, new_size); 
}
