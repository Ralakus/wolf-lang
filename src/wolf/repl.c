#include "repl.h"

#include "instance.h"
#include "util/logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util/memory.h>

bool wolf_repl(wolf_repl_params_t* params) {
    char line[1024];

    wolf_t instance;
    wolf_init(&instance);

    wolf_set_debug_mode(&instance, params->debug_mode);

    wolf_lexer_t lexer;

    for(;;) {
        wolf_lexer_init(&lexer, line);

        wolf_print("> ");

        if(!fgets(line, sizeof(line), stdin)) {
            wolf_print_raw("\n");
            break;
        }

        wolf_token_t token = wolf_lexer_scan(&lexer);

        /* is command */
        if(token.type == WOLF_TOK_COLON) {
            token = wolf_lexer_scan(&lexer);
            
            /* is command */
            if(token.type == WOLF_TOK_INDENTIFIER) {

                #define COMMAND(str) (memcmp(token.data, str, min((size_t)token.len, sizeof(str))) == 0)

                if(COMMAND("exit")) {
                    break;
                }
                else if(COMMAND("help")) {
                    wolf_noticeln("\':\' must be before every command");
                    wolf_noticeln("\'help\' prints this message");
                    wolf_noticeln("\'debug\' toggles debug mode");
                    wolf_noticeln("\'sys\' executes a command on the system");
                    wolf_noticeln("\'loadf\' loads and executes source file");
                    wolf_noticeln("\'loadb\' loads and executes bytecode file");
                    wolf_noticeln("\'exit\' exits repl");
                }
                else if(COMMAND("debug")) {
                    params->debug_mode = !params->debug_mode;
                    if(params->debug_mode) wolf_noticeln("Debug mode enabled");
                    else wolf_noticeln("Debug mode disabled");
                    wolf_set_debug_mode(&instance, params->debug_mode);
                }
                else if(COMMAND("sys")) {
                    token = wolf_lexer_scan(&lexer);
                    if(system(token.data)) {
                        wolf_errorln("Error running system command!");
                    }
                }
                else if(COMMAND("loadf")) {
                    token = wolf_lexer_scan(&lexer);
                    char* file_name = calloc(1, strlen(token.data));
                    memcpy(file_name, token.data, strlen(token.data) - 1);
                    if(!wolf_load_file(&instance, file_name)) {
                        wolf_errorln("Failed to load file: \'%s\'!", file_name);
                        free(file_name);
                        continue;
                    }
                    if(!wolf_run(&instance)) {
                        wolf_errorln("Failed to run: \'%s\'!", file_name);
                        free(file_name);
                        continue;
                    }
                    free(file_name);
                }
                else if(COMMAND("loadb")) {
                    token = wolf_lexer_scan(&lexer);
                    char* file_name = calloc(1, strlen(token.data));
                    memcpy(file_name, token.data, strlen(token.data) - 1);
                    if(!wolf_load_bytecode_file(&instance, file_name)) {
                        wolf_errorln("Failed to load file: \'%s\'!", file_name);
                        free(file_name);
                        continue;
                    }
                    if(!wolf_run(&instance)) {
                        wolf_errorln("Failed to run: \'%s\'!", file_name);
                        free(file_name);
                        continue;
                    }
                    free(file_name);
                }
                else {
                    wolf_errorln("Invalid command: \'%.*s\'!", token.len, token.data);
                }

                continue;
            }

        }

        if(!wolf_load(&instance, line)) {
            wolf_errorln("Failed to compile code!");
            wolf_unload(&instance);
        } else {
            if(!wolf_run(&instance)) {
                wolf_errorln("Error running code!");
            }
        }
    }

    wolf_free(&instance);

    return true;
}