#include <programs.h>
#include <standardLib.h>
#include <graphics.h>
#include <timeDate.h>
#include <system.h>
#include <commands.h>

#define ARGS int argc, char* argv[]

typedef int (*functionPtr)(ARGS);

struct EXECUTABLE {
    char* name;
    char* description;
    functionPtr program;
}EXECUTABLE;


typedef struct EXECUTABLE* exec;

extern void invalidOp();

void unknownCommand(char* str);

int help(ARGS), testException0(ARGS),testException6(ARGS), displayTime(ARGS), displayDate(ARGS),
sh(ARGS), playBubbles(ARGS), playPong(ARGS), playBeep(ARGS);

static exec bArr[] = {
        &(struct EXECUTABLE){"help", "displays all available commands", help},
        &(struct EXECUTABLE){"test-div0", "runs a zero division", testException0},
        &(struct EXECUTABLE){"test-invalidop", "runs an invalid op", testException6},
        &(struct EXECUTABLE){"time", "prints the current time", displayTime},
        &(struct EXECUTABLE){"date", "prints the current date", displayDate},
        &(struct EXECUTABLE){"sh", "runs the specified process", sh},
        NULL
        };

static exec pArr[] = {
        &(struct EXECUTABLE){"bubbles", "shows colored bubbles on the screen", playBubbles},
        &(struct EXECUTABLE){"pong", "runs a virtual ping pong match against the computer", playPong},
        &(struct EXECUTABLE){"beep", "produces a 'beep' sound", playBeep},
        NULL

};

int callBuiltin(int argc, char* argv[]) {
    if(argc >= 1) {
        for(int i=0; bArr[i]!=NULL; i++) {
            if(strcmp(bArr[i]->name, argv[0]) == 0) {
                return bArr[i]->program(argc, argv);
            }
        }
    }
    return 0;
}

int parseCommand(char* str) {
    if (str == NULL) {
        return -1;
    }

    int ret = -1;
    char** commands = (char**)alloc(sizeof(char*) * (MAX_PARAMS + 1));
    int i = 0;
    char *token = strtok(str, " ");

    while (token != NULL && i < MAX_PARAMS) {
        commands[i] = (char *)alloc(strlen(token) + 1);
        strcpy(commands[i], token);
        i++;
        token = strtok(NULL, " ");
    }
    commands[i] = NULL;

    ret = callBuiltin(i, commands);

    //no process was created, signal the shell to not wait for any process
    if(ret == 0) {
        ret = -1;
    }

    if(ret == -1) {
        for (int k = 0; k < i; k++) {
            dealloc((uint64_t)commands[k]);
        }
    }
    dealloc((uint64_t)commands);

    return ret;
}

void unknownCommand(char* str) {
    printFormat("\nUnknown command:\n\t'%s'\nType 'help' for a list of available commands.\n\n", str);
}

int help(ARGS) {
    for(int i=0; bArr[i] != null; i++){
        putChar('\n');
        printFormat("\t'%s': %s \n", bArr[i]->name, bArr[i]->description);
        putChar('\n');
    }
    return 0;
}

int testException0(ARGS) {
   int i=1/0;
   return 0;
}

int testException6(ARGS){
    invalidOp();
    return 0;
}

int displayTime(ARGS) {
    printTime();
    putChar('\n');
    return 0;
}

int displayDate(ARGS) {
    printDate();
    putChar('\n');
    return 0;
}

int sh(ARGS) {
    if(argc >= 2) {
        for(int i=0; pArr[i]!=NULL; i++){
            if(strcmp(pArr[i]->name, argv[1]) == 0) {
                int fg = FOREGROUND;
                for(int j=2; j<argc; j++) {
                    if(strcmp(argv[j], "&") == 0) {
                        fg = BACKGROUND;
                    }
                }
                return createProcess(pArr[i]->program, HIGH, fg, pArr[i]->name, argv + 1);
            }
        }
    }
    return -1;
}

int playBubbles(ARGS) {
    enableDoubleBuffering();
    bubbles();
    clearScreen();
    disableDoubleBuffering();
    exitProc(argc, argv);
}

int playPong(ARGS) {
    enableDoubleBuffering();
    pong();
    clearScreen();
    disableDoubleBuffering();
    exitProc(argc, argv);
}

int playBeep(ARGS) {
    play_beep(2000, 100);
    exitProc(argc, argv);
}
