#include <programs.h>
#include <standardLib.h>
#include <graphics.h>
#include <timeDate.h>
#include <system.h>
#include <commands.h>

#define ARGS int argc, char* argv[]

typedef int (*functionPtr)(ARGS);

#define SH_HELP "help"

struct EXECUTABLE {
    char* name;
    char* description;
    functionPtr program;
}EXECUTABLE;


typedef struct EXECUTABLE* exec;

extern void invalidOp();

void unknownCommand(char* str);

int help(ARGS), testException0(ARGS),testException6(ARGS), displayTime(ARGS), displayDate(ARGS),
sh(ARGS), loop(ARGS), playBubbles(ARGS), playPong(ARGS), playBeep(ARGS), repeat(ARGS), kill(ARGS), ps(ARGS);

static exec bArr[] = {
        &(struct EXECUTABLE){"help", "displays all available commands", help},
        &(struct EXECUTABLE){"test-div0", "runs a zero division", testException0},
        &(struct EXECUTABLE){"test-invalidop", "runs an invalid op", testException6},
        &(struct EXECUTABLE){"time", "prints the current time", displayTime},
        &(struct EXECUTABLE){"date", "prints the current date", displayDate},
        &(struct EXECUTABLE){"sh", "runs the specified process", sh},
        &(struct EXECUTABLE){"loop", "prints its own pid every 2 seconds", loop},
        &(struct EXECUTABLE){"kill", "kills a process given its pid", kill},
        &(struct EXECUTABLE){"ps", "shows a list of all current existing processes", ps},
        NULL
        };

static exec pArr[] = {
        &(struct EXECUTABLE){"bubbles", "shows colored bubbles on the screen", playBubbles},
        &(struct EXECUTABLE){"pong", "runs a virtual ping pong match against the computer", playPong},
        &(struct EXECUTABLE){"beep", "produces a 'beep' sound", playBeep},
        &(struct EXECUTABLE){"repeat", "prints all parameters passed", repeat},
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

    for (int k = 0; k < i; k++) {
        dealloc((void *) commands[k]);
    }

    dealloc((void *) commands);

    return ret;
}

void unknownCommand(char* str) {
    printFormat("\nUnknown command:\n\t'%s'\nType 'help' for a list of available commands.\n\n", str);
}

void unkownProcess(char* str) {
    printFormat("\nUnknown process:\n\t'%s'\nType 'sh %s' for a list of available processes.\n\n", str, SH_HELP);
}

int help(ARGS) {
    for(int i=0; bArr[i] != NULL; i++){
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

int kill(ARGS) {
    int pid = atoi(argv[1]);
    killProcess(pid);
    return 0;
}

int ps(ARGS) {
    printAllProcesses();
    putChar('\n');
    return 0;
}

int shHelp() {
    for(int i=0; pArr[i] != NULL; i++){
        putChar('\n');
        printFormat("\t'%s': %s \n", pArr[i]->name, pArr[i]->description);
        putChar('\n');
    }
    return 0;
}

int sh(ARGS) {
    //TODO hacerla mas linda
    char* proc = NULL;
    if(argc >= 2) {
        if(strcmp(argv[1], SH_HELP) == 0) {
            shHelp();
            return -1;
        }
        for(int i=0; pArr[i]!=NULL; i++) {
            if(strcmp(pArr[i]->name, argv[1]) == 0) {
                int fg = FOREGROUND;
                int lastParamPos = 2;

                for(int j=2; j<argc; j++) {
                    if(strcmp(argv[j], "&") == 0) {
                        fg = BACKGROUND;
                        if(lastParamPos == 2) {
                            lastParamPos = j + 1;
                        }
                    } else if(strcmp(argv[j], "|") == 0) {
                        //TODO add pipe code
                        if(lastParamPos == 2) {
                            lastParamPos = j + 1;
                        }
                    }
                }
                if(lastParamPos == 2) {
                    lastParamPos = argc;
                }

                //creates a copy of the parameters, excluding "sh"
                char** arguments = (char**)alloc(sizeof(char*) * lastParamPos);
                int k = 0;
                for(; k+1<lastParamPos; k++) {
                    arguments[k] = (char*)alloc(sizeof(char) * (strlen(argv[k+1]) + 1));
                    strcpy(arguments[k], argv[k+1]);
                }
                arguments[k] = NULL;
                return createProcess(pArr[i]->program, HIGH, fg, pArr[i]->name, arguments);
            }
        }
        proc = argv[1];
    } else {
        proc = " ";
    }
    unkownProcess(proc);
    return -1;
}

int loopProc(ARGS) {
    while(1) {
        wait(2000);
        printFormat("\nHello from process %d!", getOwnPid());
    }
}

int loop(ARGS) {
    return createProcess(loopProc, HIGH, FOREGROUND, "loop", NULL);
}

int playBubbles(ARGS) {
    enableDoubleBuffering();
    bubbles();
    clearScreen();
    disableDoubleBuffering();
    exitProc();
    return 0;
}

int playPong(ARGS) {
    enableDoubleBuffering();
    pong();
    clearScreen();
    disableDoubleBuffering();
    exitProc();
    return 0;
}

int playBeep(ARGS) {
    play_beep(2000, 100);
    exitProc();
    return 0;
}

int repeat(ARGS) {
    for(int i=0; i<argc; i++) {
        printFormat("\n%s ", argv[i]);
    }
    exitProc();
    return 0;
}
