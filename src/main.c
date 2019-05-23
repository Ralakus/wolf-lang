#include "util/logger.h"
#include "util/arg_parser.h"
#include "vm.h"
#include "lexer.h"
#include "parser.h"

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

static bool intrepret(const char* source) {
    wolf_vm_t vm;
    wolf_vm_init(&vm);

    wolf_lexer_t lexer;
    wolf_lexer_init(&lexer, source);

    wolf_parser_t parser;
    wolf_parser_init(&parser);

    wolf_bytecode_t bytecode;
    wolf_bytecode_init(&bytecode);

    isize_t line = -1;

    bool exit = false;
    for(;;) {
        wolf_token_t token = wolf_lexer_scan(&lexer);

        if(token.type == WOLF_TOK_ERR) {
            wolf_errorln("Error, failed to lex string!");
            break;
        }

        if(line == -1 && token.type == WOLF_TOK_INDENTIFIER) {
            if(token.len == 4) {
                if(memcmp(token.data, "exit", 4)==0) {
                    exit = true;
                    break;
                }
            }
        }

        if(token.line != line) {
            wolf_print_raw(WOLF_ANSI_RED"%4d "WOLF_ANSI_RESET, token.line);
            line = token.line;
        } else {
            wolf_print_raw(WOLF_ANSI_RED"   | "WOLF_ANSI_RESET);
        }
        if(token.type == WOLF_TOK_EOF) {
            wolf_println_raw(WOLF_ANSI_GREEN"%-14.14s "WOLF_ANSI_YELLOW"\'\\0\'"WOLF_ANSI_RESET, wolf_token_str_map[token.type]);

            wolf_parser_parse(&parser, &bytecode, source);
            wolf_bytecode_disassemble(&bytecode, "Test");

            wolf_noticeln(WOLF_ANSI_CYAN"--== Interpret ==--"WOLF_ANSI_RESET);
            wolf_vm_set_debug_trace(&vm, true);
            wolf_vm_run_bytecode(&vm, &bytecode);

            break;
        }

        wolf_println_raw(WOLF_ANSI_GREEN"%-14.14s"WOLF_ANSI_YELLOW" \'%.*s\'"WOLF_ANSI_RESET, wolf_token_str_map[token.type], token.len, token.data);

    }

    wolf_bytecode_free(&bytecode);
    wolf_parser_free(&parser);
    wolf_lexer_free(&lexer);
    wolf_vm_free(&vm);

    return !exit;
}

static void repl() {
    char line[1024];
    for(;;) {
        wolf_print("> ");

        if(!fgets(line, sizeof(line), stdin)) {
            wolf_print_raw("\n");
            break;
        }

        if(!intrepret(line)) {
            break;
        }

    }
}

int main(int argc, char* argv[]) {

    wolf_arg_parser_t arg_parser;
    wolf_arg_parser_init(&arg_parser);

    wolf_arg_t arg_help;
    wolf_arg_init(&arg_help, "h", "help", "Prints this message", false);
    wolf_arg_parser_add_arg(&arg_parser, &arg_help);


    if(!wolf_arg_parser_parse(&arg_parser, argc, (const char**)argv)) {
        wolf_errorln("Failed to parse arguments!");
        wolf_arg_free(&arg_help);
        wolf_arg_parser_free(&arg_parser);
        return 1;
    }

    if(arg_help.found) {
        wolf_arg_parser_print_help(&arg_parser, "Wolf language 0.1.0");
        wolf_arg_free(&arg_help);
        wolf_arg_parser_free(&arg_parser);
        return 0;
    }

    if(arg_parser.ea_len > 0) {
        /*
            TODO: add reading file an such
        */
    } else {
        repl();
    }

    wolf_arg_free(&arg_help);
    wolf_arg_parser_free(&arg_parser);

    return 0;
}