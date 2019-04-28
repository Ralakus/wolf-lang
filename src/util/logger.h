#pragma once

/*

    Five types of print function each with a newline varient:
        print
        notice
        warn
        error
        success

*/

#define WOLF_ANSI_RED     "\x1b[31m"
#define WOLF_ANSI_GREEN   "\x1b[32m"
#define WOLF_ANSI_YELLOW  "\x1b[33m"
#define WOLF_ANSI_BLUE    "\x1b[34m"
#define WOLF_ANSI_MAGENTA "\x1b[35m"
#define WOLF_ANSI_CYAN    "\x1b[36m"
#define WOLF_ANSI_RESET   "\x1b[0m"

#include "common.h"

/* returns a FILE* */
void* wolf_print_stream();
/* returns a FILE* */
void* wolf_error_stream();


/*  stream must be a FILE*! */
bool wolf_retarget_print(void* stream);
/* stream must be a FILE*! */
bool wolf_retarget_error(void* stream);


/* outputs to print stream, prefixed with '[ ]: ' */
void wolf_print    (const char* fmt, ...);
/* outputs to print stream, prefixed with '[ ]: ' and newlined */
void wolf_println  (const char* fmt, ...);


/* outputs to print stream, prefixed with '[-]: ' */
void wolf_notice   (const char* fmt, ...);
/* outputs to print stream, prefixed with '[-]: ' and newlined */
void wolf_noticeln (const char* fmt, ...);


/* outputs to error stream, prefixed with '[*]: ' */
void wolf_warn     (const char* fmt, ...);
/* outputs to error stream, prefixed with '[*]: ' and newlined */
void wolf_warnln   (const char* fmt, ...);


/* outputs to error stream, prefixed with '[!]: ' */
void wolf_error    (const char* fmt, ...);
/* outputs to error stream, prefixed with '[!]: ' and newlined */
void wolf_errorln  (const char* fmt, ...);


/* outputs to print stream, prefixed with '[^]: ' */
void wolf_success  (const char* fmt, ...);
/* outputs to print stream, prefixed with '[^]: ' and newlined */
void wolf_successln(const char* fmt, ...);


/* outputs to print stream */
void wolf_print_raw    (const char* fmt, ...);
/* outputs to print stream and newlined */
void wolf_println_raw  (const char* fmt, ...);


/* outputs to error stream */
void wolf_error_raw   (const char* fmt, ...);
/* outputs to print stream and newlined */
void wolf_errorln_raw (const char* fmt, ...);