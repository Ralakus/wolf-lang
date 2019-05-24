#include "util/logger.h"
#include "util/arg_parser.h"
#include "vm.h"
#include "lexer.h"
#include "parser.h"

#include "repl.h"
#include "instance.h"

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

    if(argc == 1) {
        wolf_errorln("Expected arguments!");
        wolf_arg_parser_print_help(&arg_parser, "Wolf language 0.1.0");
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

        wolf_t instance;
        wolf_init(&instance);

        wolf_set_debug_mode(&instance, arg_debug.found);

        const char* file_name = NULL;

        if(arg_parser.ea_len > 0) {
            file_name = arg_parser.extra_args[0];
        } else {
            wolf_errorln("Expected input file!");

            wolf_arg_free(&arg_help);
            wolf_arg_free(&arg_debug);
            wolf_arg_free(&arg_compile);
            wolf_arg_free(&arg_bytecode);
            wolf_arg_free(&arg_output);
            wolf_arg_free(&arg_repl);

            wolf_arg_parser_free(&arg_parser);

            return 1;
        }

        if(arg_bytecode.found) {

            if(!wolf_load_bytecode_file(&instance, file_name)) {
                wolf_errorln("Failed to load bytecode file!");

                wolf_arg_free(&arg_help);
                wolf_arg_free(&arg_debug);
                wolf_arg_free(&arg_compile);
                wolf_arg_free(&arg_bytecode);
                wolf_arg_free(&arg_output);
                wolf_arg_free(&arg_repl);

                wolf_arg_parser_free(&arg_parser);

                return 1;

            }

            if(!wolf_run(&instance)) {
                wolf_errorln("Failed to execute bytecode!");

                wolf_arg_free(&arg_help);
                wolf_arg_free(&arg_debug);
                wolf_arg_free(&arg_compile);
                wolf_arg_free(&arg_bytecode);
                wolf_arg_free(&arg_output);
                wolf_arg_free(&arg_repl);

                wolf_arg_parser_free(&arg_parser);

                return 1;
            }



        } else if(arg_compile.found) {

            const char* output = NULL;
            if(arg_output.found) {
                output = arg_output.preceeding_args[0];
            } else {
                wolf_errorln("Expected output file!");

                wolf_arg_free(&arg_help);
                wolf_arg_free(&arg_debug);
                wolf_arg_free(&arg_compile);
                wolf_arg_free(&arg_bytecode);
                wolf_arg_free(&arg_output);
                wolf_arg_free(&arg_repl);

                wolf_arg_parser_free(&arg_parser);

                return 1;
            }

            if(!wolf_load_file(&instance, file_name)) {
                wolf_errorln("Failed to load source file!");
            }

            size_t slen = 0;
            uint8_t* serialized = wolf_bytecode_serialize(&instance.bytecode, &slen, arg_debug.found);

            if(arg_debug.found) {
                wolf_noticeln("Serlaized bytecode: ");
                for(isize_t i = 0; (size_t)i < slen; i++) {
                    wolf_print_raw("%02hhx ", serialized[i]);
                }
                wolf_print_raw("\n");
            }

            wolf_unload(&instance);

            FILE* out = fopen(output, "wb+");
            if(out == NULL){
                wolf_errorln("Failed to open '%s'!", output);
                return false;
            }

            fwrite(serialized, slen, 1, out);
            fclose(out);

            free(serialized);

        } else {
            if(!wolf_load_file(&instance, file_name)) {
                wolf_errorln("Failed to load source file!");

                wolf_arg_free(&arg_help);
                wolf_arg_free(&arg_debug);
                wolf_arg_free(&arg_compile);
                wolf_arg_free(&arg_bytecode);
                wolf_arg_free(&arg_output);
                wolf_arg_free(&arg_repl);

                wolf_arg_parser_free(&arg_parser);

                return 1;

            }

            if(!wolf_run(&instance)) {
                wolf_errorln("Failed to execute bytecode!");

                wolf_arg_free(&arg_help);
                wolf_arg_free(&arg_debug);
                wolf_arg_free(&arg_compile);
                wolf_arg_free(&arg_bytecode);
                wolf_arg_free(&arg_output);
                wolf_arg_free(&arg_repl);

                wolf_arg_parser_free(&arg_parser);

                return 1;
            }
        }

        wolf_free(&instance);

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