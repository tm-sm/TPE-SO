#ifndef TPE_ARQUI_COMMANDS_H
#define TPE_ARQUI_COMMANDS_H

#define MAX_PARAMS 16

/**
 * Receives a string and executes functions with a matching name stored in the commands.c file
 * @param str String to parse
 */
int parseCommand(char* str);

#endif //TPE_ARQUI_COMMANDS_H
