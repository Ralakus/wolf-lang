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

                #define COMMAND(str) (memcmp(token.data, str, min(token.len, sizeof(str))) == 0)

                if(COMMAND("exit")) {
                    break;
                }
                else if(COMMAND("help")) {
                    wolf_noticeln("\':\' must be before every command");
                    wolf_noticeln("\'help\' prints this message");
                    wolf_noticeln("\'debug\' toggles debug mode");
                    wolf_noticeln("\'sys\' executes a command on the system");
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
                } else {
                    wolf_errorln("Invalid command: \'%.*s\'!", token.len, token.data);
                }

                continue;
            }

        }

        if(!wolf_load(&instance, line)) {
            wolf_errorln("Failed to compile code!");
            wolf_unload(&instance);
        } else {
            if(params->debug_mode) {
                wolf_noticeln(WOLF_ANSI_CYAN"--== VM Stacktrace ==--"WOLF_ANSI_RESET);
            }
            if(!wolf_run(&instance)) {
                wolf_errorln("Error running code!");
            }
        }
    }

    wolf_free(&instance);

    return true;
}