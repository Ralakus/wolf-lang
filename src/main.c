#include "util/logger.h"
#include "util/arg_parser.h"
#include "vm.h"

static void repl() {
    char line[1024];
    for(;;) {
        wolf_print("> ");
    }
}

int main(int argc, char* argv[]) {

    wolf_arg_parser_t arg_parser;
    wolf_arg_parser_init(&arg_parser);

    wolf_arg_t arg_help;
    wolf_arg_init(&arg_help, "h", "help", "Prints this message", false);
    wolf_arg_parser_add_arg(&arg_parser, &arg_help);


    if(!wolf_arg_parser_parse(&arg_parser, argc, argv)) {
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

    wolf_bytecode_t bytecode;
    wolf_bytecode_init(&bytecode);

    isize_t constant = wolf_bytecode_write_constant(&bytecode, 14.0);
    wolf_bytecode_write(&bytecode, WOLF_OP_CONSTANT, 0);
    wolf_bytecode_write(&bytecode, constant, 0);

    wolf_bytecode_write(&bytecode, WOLF_OP_NEGATE, 0);

    constant = wolf_bytecode_write_constant(&bytecode, 48.0);
    wolf_bytecode_write(&bytecode, WOLF_OP_CONSTANT, 0);
    wolf_bytecode_write(&bytecode, constant, 0);

    wolf_bytecode_write(&bytecode, WOLF_OP_NEGATE, 0);

    wolf_bytecode_write(&bytecode, WOLF_OP_MUL, 0);
    
    wolf_bytecode_write(&bytecode, WOLF_OP_RETURN, 0);

    wolf_bytecode_disassemble(&bytecode, "Test");

    wolf_vm_t vm;
    wolf_vm_init(&vm);
    wolf_vm_set_debug_trace(&vm, true);

    wolf_noticeln(WOLF_ANSI_CYAN"--== Virtual Machine Trace ==--"WOLF_ANSI_RESET);
    wolf_vm_run_bytecode(&vm, &bytecode);

    wolf_vm_free(&vm);

    wolf_bytecode_free(&bytecode);

    return 0;
}