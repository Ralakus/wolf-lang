#include "util/logger.h"

#include "vm.h"

int main(int argc, char* argv[]) {
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