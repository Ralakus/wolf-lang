#include "util/logger.h"
#include "util/arg_parser.h"
#include "vm.h"
#include "lexer.h"
#include "parser.h"

#include "repl.h"

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

    wolf_arg_parser_t arg_parser;
    wolf_arg_parser_init(&arg_parser);

    wolf_arg_t arg_help;
    wolf_arg_init(&arg_help, "h", "help", "Displays this help message", false);
    wolf_arg_parser_add_arg(&arg_parser, &arg_help);

    wolf_arg_t arg_debug;
    wolf_arg_init(&arg_debug, "d", "debug", "Enables debug printout from lexer, parser, and virtual machine", false);
    wolf_arg_parser_add_arg(&arg_parser, &arg_debug);

    wolf_arg_t arg_compile;
    wolf_arg_init(&arg_compile, "c", "compile", "Compiles source files into bytecode files", false);
    wolf_arg_parser_add_arg(&arg_parser, &arg_compile);

    wolf_arg_t arg_bytecode;
    wolf_arg_init(&arg_bytecode, "b", "bytecode", "Runs bytecode from input, does not lex or parse", false);
    wolf_arg_parser_add_arg(&arg_parser, &arg_bytecode);

    wolf_arg_t arg_output;
    wolf_arg_init(&arg_output, "o", "output", "Outputs to file", true);
    wolf_arg_parser_add_arg(&arg_parser, &arg_output);

    wolf_arg_t arg_repl;
    wolf_arg_init(&arg_repl, "r", "repl", "Starts up in repl environment", false);
    wolf_arg_parser_add_arg(&arg_parser, &arg_repl);

    if(!wolf_arg_parser_parse(&arg_parser, argc, (const char**)argv)) {
        wolf_errorln("Failed to parse arguments!");
        wolf_arg_free(&arg_help);
        wolf_arg_parser_free(&arg_parser);
        return 1;
    }

    if(arg_help.found) {
        wolf_arg_parser_print_help(&arg_parser, "Wolf language 0.1.0");

        wolf_arg_free(&arg_help);
        wolf_arg_free(&arg_debug);
        wolf_arg_free(&arg_compile);
        wolf_arg_free(&arg_bytecode);
        wolf_arg_free(&arg_output);
        wolf_arg_free(&arg_repl);

        wolf_arg_parser_free(&arg_parser);

        return 0;
    }

    if(arg_repl.found) {
        wolf_repl_params_t params = (wolf_repl_params_t){
            .debug_mode = arg_debug.found
        };
        wolf_repl(&params);
    } else {
        wolf_errorln("Nothing else implemented yet!");

        wolf_arg_free(&arg_help);
        wolf_arg_free(&arg_debug);
        wolf_arg_free(&arg_compile);
        wolf_arg_free(&arg_bytecode);
        wolf_arg_free(&arg_output);
        wolf_arg_free(&arg_repl);

        wolf_arg_parser_free(&arg_parser);

        return 1;
    }

    wolf_arg_free(&arg_help);
    wolf_arg_free(&arg_debug);
    wolf_arg_free(&arg_compile);
    wolf_arg_free(&arg_bytecode);
    wolf_arg_free(&arg_output);
    wolf_arg_free(&arg_repl);

    wolf_arg_parser_free(&arg_parser);

    return 0;
}