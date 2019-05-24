#include "arg_parser.h"

#include <string.h>

#include "memory.h"
#include "logger.h"


/* ================================

    Argument type

================================ */ 

void wolf_arg_init(wolf_arg_t* this, const char* short_name, const char* long_name, const char* description, bool expect_preceeding) {
    this->short_name  = short_name;
    this->long_name   = long_name;
    this->description = description;

    this->expect_preceeding = expect_preceeding;

    this->found = false;

    this->pa_alloc_len    = 0;
    this->pa_len          = 0;
    this->preceeding_args = NULL;
}

void wolf_arg_free(wolf_arg_t* this) {
    WOLF_FREE_ARRAY(const char*, this->preceeding_args, this->pa_alloc_len);
}

void write_preceeding_arg(wolf_arg_t* this, const char* arg) {
    if(this->pa_alloc_len < this->pa_len + 1) {
        isize_t old_alloc_len = this->pa_alloc_len;
        this->pa_alloc_len = WOLF_GROW_CAPACITY(old_alloc_len);
        this->preceeding_args = WOLF_GROW_ARRAY(const char*,
                                this->preceeding_args,
                                old_alloc_len,
                                this->pa_alloc_len);
    }

    this->preceeding_args[this->pa_len] = arg;
    ++this->pa_len;
}

/* ================================

    Argument parser

================================ */ 


void wolf_arg_parser_init(wolf_arg_parser_t* this) {
    this->a_alloc_len = 0;
    this->a_len       = 0;
    this->args        = NULL;

    this->ea_alloc_len = 0;
    this->ea_len       = 0;
    this->extra_args   = NULL;
}

void wolf_arg_parser_free(wolf_arg_parser_t* this) {
    WOLF_FREE_ARRAY(wolf_arg_t*, this->args, this->a_alloc_len);
    WOLF_FREE_ARRAY(const char*, this->extra_args, this->ea_alloc_len);
    wolf_arg_parser_init(this);
}

void wolf_arg_parser_add_arg(wolf_arg_parser_t* this, wolf_arg_t* arg) {
    if(this->a_alloc_len < this->a_len + 1) {
        isize_t old_alloc_len = this->a_alloc_len;
        this->a_alloc_len = WOLF_GROW_CAPACITY(old_alloc_len);
        this->args = WOLF_GROW_ARRAY(wolf_arg_t*,
                     this->args,
                     old_alloc_len,
                     this->a_alloc_len);
    }

    this->args[this->a_len] = arg;
    ++this->a_len;
}

static void add_extra_arg(wolf_arg_parser_t* this, const char* arg) {
    if(this->ea_alloc_len < this->ea_len + 1) {
        isize_t old_alloc_len = this->ea_alloc_len;
        this->ea_alloc_len = WOLF_GROW_CAPACITY(old_alloc_len);
        this->extra_args = WOLF_GROW_ARRAY(const char*,
                           this->extra_args,
                           old_alloc_len,
                           this->ea_alloc_len);
    }

    this->extra_args[this->ea_len] = arg;
    ++this->ea_len;
}

bool wolf_arg_parser_parse  (wolf_arg_parser_t* this, int argc, const char** argv) {
    wolf_arg_t* prev_arg = NULL;

    for(int i = 1; i < argc; i++) {

        bool arg_found = false;

        if(argv[i][0] == '-') {

            if(argv[i][1] == '-') {

                /*
                    Long argument
                */

                for(isize_t j = 0; j < this->a_len; j++) {

                    wolf_arg_t* cur_arg = this->args[j];

                    if(strcmp(cur_arg->long_name, &argv[i][2])==0) {
                        cur_arg->found = true;
                        prev_arg = cur_arg;
                        arg_found = true;
                        break;
                    }
                }
            } else {
                
                /*
                    Short argument
                */

                size_t arg_len = strlen(argv[i]);

                for(size_t j = 0; j < arg_len; j++) {

                    for(isize_t k = 0; k < this->a_len; k++) {

                        wolf_arg_t* cur_arg = this->args[k];

                        if(
                            memcmp(cur_arg->short_name, &argv[i][j],
                            min(
                            strlen(cur_arg->short_name), strlen(&argv[i][j])
                            ))==0) {
                            cur_arg->found = true;
                            prev_arg = cur_arg;
                            arg_found = true;
                            break;
                        }
                    }
                }
            }

            if(!arg_found) {
                wolf_errorln("Unexpeced argument: \'%s\'!", argv[i]);
                return false;
            }
        } else {

            /*
                Preceeding argument
            */

           if(prev_arg == NULL) {
               add_extra_arg(this, argv[i]);
           } else {
               if(prev_arg->expect_preceeding) {
                   write_preceeding_arg(prev_arg, argv[i]);
               } else {
                   add_extra_arg(this, argv[i]);
               }
           }
        }
    }

    return true;
}

void wolf_arg_parser_print_help(wolf_arg_parser_t* this, const char* program_description) {
    wolf_noticeln("%s", program_description);
    for(isize_t i = 0; i < this->a_len; i++) {
        wolf_noticeln("    -%s, --%-10s %s",
            this->args[i]->short_name,
            this->args[i]->long_name,
            this->args[i]->description
        );
    }
}