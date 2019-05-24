#include "instance.h"

#include "util/logger.h"

#include <stdlib.h>
#include <stdio.h>

static char* read_file(const char* file_name, bool binary) {
    char* file_contents = NULL;

    FILE* file = fopen(file_name, binary ? "rb" : "r");
    if(file == NULL){
        wolf_errorln("Failed to open '%s'!", file_name);
        return NULL;
    }

    size_t file_size = 0;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file) + 1;
    rewind(file);

    file_contents = malloc(file_size + 1);
    if(file_contents == NULL) {
        wolf_errorln("Failed to allocate buffer for '%s'!", file_name);
        return NULL;
    }

    if(fread(file_contents, 1, file_size, file) < file_size - 1) {
        wolf_errorln("Failed to read '%s'!", file_name);
        free(file_contents);
        return NULL;
    }

    file_contents[file_size - 1] = '\0';

    fclose(file);

    return file_contents;
}

void wolf_init(wolf_t* this) {
    this->debug_mode  = false;
    this->loaded      = false;
    this->file_loaded = false;
    this->source      = NULL;
    wolf_parser_init(&this->parser);
    wolf_bytecode_init(&this->bytecode);
    wolf_vm_init(&this->vm);
}

void wolf_free(wolf_t* this) {
    this->loaded      = false;
    if(this->file_loaded) {
        free((char*)this->source);
        this->file_loaded = false;
    }
    this->source      = NULL;
    wolf_parser_free(&this->parser);
    wolf_bytecode_free(&this->bytecode);
    wolf_vm_free(&this->vm);
}

void wolf_set_debug_mode(wolf_t* this, bool value) {
    this->debug_mode = value;
    this->parser.debug_mode = this->debug_mode;
    wolf_vm_set_debug_trace(&this->vm, this->debug_mode);
}

bool wolf_load(wolf_t* this, const char* code) {
    wolf_unload(this);
    if(code==NULL) return false;
    this->loaded      = true;
    this->file_loaded = false;
    this->source      = code;

    bool result = wolf_parser_parse(&this->parser, &this->bytecode, this->source);
    if(!result) {
        return false;
    } else {
        if(this->debug_mode) {
            wolf_bytecode_disassemble(&this->bytecode, "Input source code");
        }
        return true;
    }
}
bool wolf_load_file(wolf_t* this, const char* file_name) {
    wolf_unload(this);
    return false;
}

bool wolf_load_bytecode(wolf_t* this, wolf_bytecode_t bytecode) {
    wolf_unload(this);
    this->loaded      = true;
    this->file_loaded = false;
    this->bytecode = bytecode;
    if(this->debug_mode) {
        wolf_bytecode_disassemble(&this->bytecode, "Input bytecode");
    }
    return true;
}
bool wolf_load_bytecode_file(wolf_t* this, const char* file_name) {
    wolf_unload(this);
    return false;
}

bool wolf_unload(wolf_t* this) {
    this->loaded = false;
    this->source = NULL;
    wolf_bytecode_free(&this->bytecode);
    return true;
}

bool wolf_run(wolf_t* this) {
    if(this->loaded) {
        wolf_interpret_result_t result = wolf_vm_run_bytecode(&this->vm, &this->bytecode);
        if(result != WOLF_INTERPRET_OK) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}
