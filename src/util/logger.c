#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "logger.h"

#define PRINT_PREFIX   WOLF_ANSI_RESET  "[ ]: " WOLF_ANSI_RESET
#define NOTICE_PREFIX  WOLF_ANSI_CYAN   "[-]: " WOLF_ANSI_RESET
#define WARN_PREFIX    WOLF_ANSI_YELLOW "[*]: " WOLF_ANSI_RESET
#define ERROR_PREFIX   WOLF_ANSI_RED    "[!]: " WOLF_ANSI_RESET
#define SUCCESS_PREFIX WOLF_ANSI_GREEN  "[^]: " WOLF_ANSI_RESET

#define PRINT_TEMPLATE(target, exec) va_list args;                       \
                                           va_start(args, fmt);                \
                                           FILE* print_stream = (FILE*)target; \
                                           exec                                \
                                           va_end(args)

static FILE* print_stream_ptr = NULL;
static FILE* error_stream_ptr = NULL;



void* wolf_print_stream() {
    if(print_stream_ptr==NULL) {
        print_stream_ptr = stdout;
    }
    return print_stream_ptr;
}
void* wolf_error_stream() {
    if(error_stream_ptr==NULL) {
        error_stream_ptr = stderr;
    }
    return error_stream_ptr;
}
bool wolf_retarget_print(void* target) {
    if(target==NULL) {
        return false;
    } else {
        print_stream_ptr = (FILE*)target;
        return true;
    }
}
bool wolf_retarget_error(void* target) {
    if(target==NULL) {
        return false;
    } else {
        error_stream_ptr = (FILE*)target;
        return true;
    }
}

void wolf_print    (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_print_stream(),

    fprintf(print_stream,PRINT_PREFIX);
    vfprintf(print_stream, fmt, args);

    );
}
void wolf_println  (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_print_stream(),

    fprintf(print_stream,PRINT_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");

    );
}
void wolf_notice   (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_print_stream(),

    fprintf(print_stream,NOTICE_PREFIX);
    vfprintf(print_stream, fmt, args);
    
    );
}
void wolf_noticeln (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_print_stream(),

    fprintf(print_stream,NOTICE_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");
    
    );
}
void wolf_warn     (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_error_stream(),

    fprintf(print_stream,WARN_PREFIX);
    vfprintf(print_stream, fmt, args);
    
    );
}
void wolf_warnln   (const char* fmt, ...) { 
    PRINT_TEMPLATE(wolf_error_stream(),

    fprintf(print_stream,WARN_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");
    
    );
}
void wolf_error    (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_error_stream(),

    fprintf(print_stream,ERROR_PREFIX);
    vfprintf(print_stream, fmt, args);
    
    );
}
void wolf_errorln  (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_error_stream(),

    fprintf(print_stream,ERROR_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");
    
    );
}
void wolf_success (const char* fmt, ...) { 
    PRINT_TEMPLATE(wolf_print_stream(),

    fprintf(print_stream,SUCCESS_PREFIX);
    vfprintf(print_stream, fmt, args);
    
    );
}
void wolf_successln(const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_print_stream(),

    fprintf(print_stream,SUCCESS_PREFIX);
    vfprintf(print_stream, fmt, args);
    fprintf(print_stream, "\n");
    
    );
}

void wolf_print_raw    (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_print_stream(),

    vfprintf(print_stream, fmt, args);

    );
}

void wolf_println_raw  (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_print_stream(),

    vfprintf(print_stream, fmt, args);
    fprintf (print_stream, "\n");

    );
}

void wolf_error_raw   (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_error_stream(),

    vfprintf(print_stream, fmt, args);

    );
}
void wolf_errorln_raw (const char* fmt, ...) {
    PRINT_TEMPLATE(wolf_error_stream(),

    vfprintf(print_stream, fmt, args);
    fprintf (print_stream, "\n");

    );
}