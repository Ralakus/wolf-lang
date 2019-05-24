#pragma once

#include <stdbool.h> 
#include <stddef.h>  
#include <stdint.h> 

typedef ptrdiff_t isize_t;

#define UNREACHABLE() \
    ({ \
        wolf_errorln("[%s:%d]: This code should not be reached in %s()", __FILE__, __LINE__, __func__); \
        abort(); \
    })
