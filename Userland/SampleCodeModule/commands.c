#include <programs.h>
#include <standardLib.h>
#include <graphics.h>
#include <timeDate.h>
#include <system.h>
#include <commands.h>

#define SENTINEL_PID 0
#define INIT_PID 1
#define SHELL_PID 2

#define ARGS int argc, char* argv[]

typedef int (*functionPtr)(ARGS);

#define SH_HELP "help"

struct EXECUTABLE {
    char* name;
    char* description;
    functionPtr program;
}EXECUTABLE;

//TODO agregar mensajes de error para parametros mal pasados

typedef struct EXECUTABLE* exec;

extern void invalidOp();

void unknownCommand(char* str);

int help(ARGS), testException0(ARGS),testException6(ARGS), displayTime(ARGS), displayDate(ARGS), mem(ARGS),
sh(ARGS), cat(ARGS), loop(ARGS), playBubbles(ARGS), playPong(ARGS), playBeep(ARGS), repeat(ARGS), kill(ARGS),
ps(ARGS), nice(ARGS), block(ARGS),initPhyloReunion(ARGS);

static exec bArr[] = {
        &(struct EXECUTABLE){"help", "displays all available commands", help},
        &(struct EXECUTABLE){"test-div0", "runs a zero division", testException0},
        &(struct EXECUTABLE){"test-invalidop", "runs an invalid op", testException6},
        &(struct EXECUTABLE){"time", "prints the current time", displayTime},
        &(struct EXECUTABLE){"date", "prints the current date", displayDate},
        &(struct EXECUTABLE){"mem", "prints available memory in bytes", mem},
        &(struct EXECUTABLE){"sh", "runs the specified process", sh},
        &(struct EXECUTABLE){"cat", "prints the output of the specified process", cat},
        &(struct EXECUTABLE){"kill", "kills a process given its pid", kill},
        &(struct EXECUTABLE){"ps", "shows a list of all current existing processes", ps},
        &(struct EXECUTABLE){"nice", "changes a process priority given its pid: 0->HIGH 1->MED 2->LOW", nice},
        &(struct EXECUTABLE){"block", "blocks or unblocks a process given its pid", block},
        NULL
        };

static exec pArr[] = {
        &(struct EXECUTABLE){"bubbles", "shows colored bubbles on the screen", playBubbles},
        &(struct EXECUTABLE){"pong", "runs a virtual ping pong match against the computer", playPong},
        &(struct EXECUTABLE){"beep", "produces a 'beep' sound", playBeep},
        &(struct EXECUTABLE){"repeat", "prints all parameters passed", repeat},
        &(struct EXECUTABLE){"loop", "prints its own pid every 2 seconds", loop},
        &(struct EXECUTABLE){"phylo", "runs the dining philosophers problem", initPhyloReunion},
        NULL
};

int callBuiltin(int argc, char* argv[]) {
    if (argc >= 1) {
        for (int i = 0; bArr[i] != NULL; i++) {
            if (strcmp(bArr[i]->name, argv[0]) == 0) {
                return bArr[i]->program(argc, argv);
            }
        }
        unknownCommand(argv[0]);
        return 0;
    }
    unknownCommand(" ");
    return 0;
}

int createProcessWithParams(exec p, int priority, int fg, int isBlocked, char* argv[], int from, int to) {
    int argc = (to - from) + 1;
    char** arguments = (char**)alloc(sizeof(char*) * argc);
    int k = 0;
    for(; k+1<argc; k++) {
        arguments[k] = (char*)alloc(sizeof(char) * (strlen(argv[from + k]) + 1));
        strcpy(arguments[k], argv[from + k]);
    }
    arguments[k] = NULL;
    return createProcess(p->program, priority, fg, isBlocked, p->name, arguments);
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

int mem(ARGS) {
    printFormat("\nAvailable Memory:\n%d bytes\n", getAvailableMemory());
    return 0;
}

int kill(ARGS) {
    if(argc == 2) {
        int pid = atoi(argv[1]);
        if(pid == SENTINEL_PID || pid == INIT_PID || pid == SHELL_PID || pid < 0) {
            printFormat("\nForbidden\n");
            return -1;
        }
        killProcess(pid);
    }
    return 0;
}

int ps(ARGS) {
    printAllProcesses();
    putChar('\n');
    return 0;
}

int nice(ARGS) {
    if(argc == 3) {
        int pid = atoi(argv[1]);
        int priority = atoi(argv[2]);
        if(priority <= 3 && priority >= 0) {
            setProcessPriority(pid, priority);
        }
    }
    return 0;
}

int block(ARGS) {
    if(argc == 2) {
        int pid = atoi(argv[1]);
        if(isProcessBlocked(pid)) {
            unblockProcess(pid);
        } else {
            blockProcess(pid);
        }
    }
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
        int paramStart = 1;
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
                int ret = createProcessWithParams(pArr[i], HIGH, fg, 0, argv, paramStart, lastParamPos);
                if(fg == BACKGROUND) {
                    return -1;
                }
                return ret;
            }
        }
        proc = argv[1];
    } else {
        proc = " ";
    }
    unkownProcess(proc);
    return -1;
}

int catProc(ARGS) {
    int size = 128;
    char* buff = alloc(size);
    waitSem("catSem");
    size_t read = getStrn(buff, size);
    while(buff[0] != -1 && read != 0) {
        putStrn(buff);
        getStrn(buff, size);
    }
    exitProc();
    return 0;
}

int cat(ARGS) {
    if(argc >= 2) {
        for(int i=0; pArr[i] != NULL; i++) {
            if(strcmp(pArr[i]->name, argv[1]) == 0){
                openSem("catSem", 0);
                int catPid = createProcess(catProc, HIGH, FOREGROUND, 0, "cat", NULL);
                int pid = createProcessWithParams(pArr[i], LOW, BACKGROUND, 1, argv, 1, argc);
                connectProcesses(pid, catPid);
                unblockProcess(pid);
                postSem("catSem");
                return catPid;
            }
        }
    }
    return -1;
}

int loop(ARGS) {
    while(1) {
        printFormat("\nHello from process %d!", getOwnPid());
        wait(2000);
    }
    exitProc();
    return 0;
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
    printFormat("\n");
    exitProc();
    return 0;
}


int wcCount(char * string){
    int counter = 0;
    for(int i = 0;i < strlen(string); i ++){
        char c = string[i];
            if(c == '\n'){
                counter += 1;
            }
    }
    return counter;
}

static int isVowel(char c){
    return c == 'a' || c == 'A' || c == 'e'|| c == 'E' || c == 'i' || c == 'I' || c == 'o'|| c == 'O' || c == 'u' || c == 'U';
}

int filterUser(char * string){
    for(int i = 0;i < strlen(string); i ++){
        char c = string[i];
        if(!isVowel(c)){
            putChar(c);
        }
    }
    return 0;
}
