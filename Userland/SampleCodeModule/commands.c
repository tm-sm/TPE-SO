#include <programs.h>
#include <standardLib.h>
#include <graphics.h>
#include <timeDate.h>
#include <system.h>
#include <commands.h>

#define SENTINEL_PID 0
#define INIT_PID 1
#define SHELL_PID 2

#define START_IN_BACKGROUND_SYMBOL "&"
#define CONNECT_WITH_PIPE_SYMBOL "/"
#define BUFFER_SIZE 100

#define ARGS int argc, char* argv[]

typedef int (*functionPtr)(ARGS);

#define SH_HELP "help"

struct Executable {
    char* name;
    char* description;
    functionPtr program;
}Executable;

//TODO agregar mensajes de error para parametros mal pasados

typedef struct Executable* exec;

extern void invalidOp();

void unknownCommand(char* str);
int test_mm(ARGS);
int help(ARGS), testException0(ARGS),testException6(ARGS), displayTime(ARGS), displayDate(ARGS), mem(ARGS),
sh(ARGS), cat(ARGS), wc(ARGS), filter(ARGS), loop(ARGS), playBubbles(ARGS), playPong(ARGS), playBeep(ARGS), repeat(ARGS), kill(ARGS),
ps(ARGS), nice(ARGS), block(ARGS), phylo(ARGS), monologue(ARGS), quietChild(ARGS), loudChild(ARGS), stressTest(ARGS), displayFIFOList(ARGS),
connectProcsToFIFO(ARGS), testMemory(ARGS);

static exec bArr[] = {
        &(struct Executable){"help", "displays all available commands", help},
        /* deprecated for now, as proper exception handling hasn't been implemented with processes
        &(struct Executable){"test-div0", "runs a zero division", testException0},
        &(struct Executable){"test-invalidop", "runs an invalid op", testException6},
         */
        &(struct Executable){"time", "prints the current time", displayTime},
        &(struct Executable){"date", "prints the current date", displayDate},
        &(struct Executable){"mem", "prints available memory in bytes", mem},
        &(struct Executable){"sh", "runs the specified process", sh},
        &(struct Executable){"cat", "prints the output of the specified process", cat},
        &(struct Executable){"wc", "prints the number of lines written by the specified process", wc},
        &(struct Executable){"filter", "prints the output of the specified process excluding vowels", filter},
        &(struct Executable){"kill", "kills a process given its pid", kill},
        &(struct Executable){"ps", "shows a list of all current existing processes", ps},
        &(struct Executable){"nice", "changes a process priority given its pid: 0->HIGH 1->MED 2->LOW", nice},
        &(struct Executable){"block", "blocks or unblocks a process given its pid", block},
        &(struct Executable){"FIFO", "Displays the FIFO list", displayFIFOList},
        &(struct Executable){"connectFIFO", "connects two process to each end of a FIFO, requires pids and a FIFO", connectProcsToFIFO},
        NULL
        };

static exec pArr[] = {
        &(struct Executable){"bubbles", "shows colored bubbles on the screen", playBubbles},
        &(struct Executable){"pong", "runs a virtual ping pong match against the computer", playPong},
        &(struct Executable){"beep", "produces a 'beep' sound", playBeep},
        &(struct Executable){"repeat", "prints all parameters passed", repeat},
        &(struct Executable){"loop", "prints its own pid every 2 seconds", loop},
        &(struct Executable){"phylo", "runs the dining philosophers problem", phylo},
        &(struct Executable){"monologue", "starts two processes with different priority, stops once the quiet child speaks", monologue},
        &(struct Executable){"testmm","tests memory management",testMemory},
        &(struct Executable){"stressTest", "creates the maximum amount of process, incrementally blocks them and finally kills them", stressTest},
        NULL
};

int callBuiltin(int argc, char* argv[]) {
    if(argc >= 1) {
        for(int i = 0; bArr[i] != NULL; i++) {
            if(strcmp(bArr[i]->name, argv[0]) == 0) {
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
    if(str == NULL) {
        return -1;
    }

    int ret = -1;
    char** commands = (char**)alloc(sizeof(char*) * (MAX_PARAMS + 1));
    int i = 0;
    char *token = strtok(str, " ");

    while(token != NULL && i < MAX_PARAMS) {
        commands[i] = (char *)alloc(strlen(token) + 1);
        strcpy(commands[i], token);
        i++;
        token = strtok(NULL, " ");
    }
    commands[i] = NULL;

    ret = callBuiltin(i, commands);

    //no process was created, signal the shell to not wait for any process

    for(int k = 0; k < i; k++) {
        dealloc((void *) commands[k]);
    }

    dealloc((void *) commands);

    return ret;
}

void unknownCommand(char* str) {
    printFormat("\nUnknown command:\n\t'%s'\nType 'help' for a list of available commands.\n\n", str);
}

void unknownProcess(char* str) {
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
    printFormat("\nTotal Memory: %d bytes\nAvailable Memory: %d bytes\nOccupied Memory: %d bytes \n", 65536 ,getAvailableMemory(), 65536 - getAvailableMemory());
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
        if(pid == SENTINEL_PID || pid == INIT_PID || pid == SHELL_PID || pid < 0) {
            printFormat("\nForbidden\n");
            return -1;
        }
        if(priority <= 3 && priority >= 0) {
            setProcessPriority(pid, priority);
            return 0;
        }
    }
    printFormat("\nInvalid parameters\n");
    return -1;
}

int block(ARGS) {
    if(argc == 2) {
        int pid = atoi(argv[1]);
        if(pid == SENTINEL_PID || pid == INIT_PID || pid == SHELL_PID || pid < 0) {
            printFormat("\nForbidden\n");
            return -1;
        }
        if(isProcessBlocked(pid)) {
            unblockProcess(pid);
        } else {
            blockProcess(pid);
        }
    }
    printFormat("\nInvalid parameters\n");
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

int sh(int argc, char* argv[]) {
    char* proc = NULL;
    if(argc >= 2) {
        int paramStart = 1;
        if(strcmp(argv[1], SH_HELP) == 0) {
            shHelp();
            return -1;
        }

        for(int i = 0; pArr[i] != NULL; i++) {
            if(strcmp(pArr[i]->name, argv[1]) == 0) {
                int fg = FOREGROUND;
                int paramEnd = 2;

                for(int j = 2; j < argc; j++) {
                    if(strcmp(argv[j], START_IN_BACKGROUND_SYMBOL) == 0) {
                        fg = BACKGROUND;
                        if(paramEnd == 2) {
                            paramEnd = j + 1;
                        }
                    } else if(strcmp(argv[j], CONNECT_WITH_PIPE_SYMBOL) == 0) {
                        int ret1 = createProcessWithParams(pArr[i], HIGH, BACKGROUND, 1, argv, paramStart, paramEnd);

                        if(ret1 == -1) {
                            printFormat("\nProcess creation failed\n");
                            return -1;
                        }

                        if(argc == j) {
                            killProcess(ret1);
                            unknownProcess(" ");
                            return -1;
                        }
                        paramStart = j + 1;
                        paramEnd = argc;

                        for(int k = 0; pArr[k] != NULL; k++) {
                            if(strcmp(pArr[k]->name, argv[j + 1]) == 0) {
                                int ret2 = createProcessWithParams(pArr[k], HIGH, fg, 1, argv, paramStart, paramEnd);

                                if(ret2 == -1) {
                                    killProcess(ret1);
                                    printFormat("\nProcess creation failed\n");
                                    return -1;
                                }

                                connectProcesses(ret1, ret2);
                                unblockProcess(ret1);
                                unblockProcess(ret2);
                                if(fg == BACKGROUND) {
                                    return -1;
                                }

                                return ret2;
                            }
                        }

                        killProcess(ret1);
                        unknownProcess(argv[paramStart]);
                        return -1;
                    }
                }

                if(paramEnd == 2) {
                    paramEnd = argc;
                }

                int ret = createProcessWithParams(pArr[i], HIGH, fg, 0, argv, paramStart, paramEnd);
                if(ret == -1) {
                    printFormat("\nProcess creation failed\n");
                    return -1;
                }
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
    unknownProcess(proc);
    return -1;
}

int catProc(ARGS) {
    int size = BUFFER_SIZE;
    char* buff = alloc(size);
    size_t read = getStrn(buff, size);
    while(buff[0] != -1 && read != 0) {
        putStrn(buff);
        getStrn(buff, size);
    }
    dealloc(buff);
    exitProc();
    return 0;
}

int cat(ARGS) {
    if(argc >= 2) {
        for(int i=0; pArr[i] != NULL; i++) {
            if (strcmp(pArr[i]->name, argv[1]) == 0) {
                int catPid = createProcess(catProc, HIGH, FOREGROUND, 1, "cat", NULL);
                int pid = createProcessWithParams(pArr[i], LOW, BACKGROUND, 1, argv, 1, argc);
                connectProcesses(pid, catPid);
                unblockProcess(pid);
                unblockProcess(catPid);
                return catPid;
            }
        }
        unknownProcess(argv[1]);
    }
    return -1;
}

int wcProc(ARGS) {
    int counter = 0;
    char *buff = alloc(BUFFER_SIZE);
    size_t len = getStrn(buff, BUFFER_SIZE);
    while(len != 0 && buff[0] != -1) {
        for(int i = 0; buff[i] != '\0'; i++) {
            if(buff[i] == '\n') {
                counter++;
            }
        }
        len = getStrn(buff, BUFFER_SIZE);
    }
    printFormat("%d lines", counter);
    dealloc(buff);
    exitProc();
    return 0;
}

int wc(ARGS) {
    if(argc >= 2) {
        for(int i=0; pArr[i] != NULL; i++) {
            if(strcmp(pArr[i]->name, argv[1]) == 0){
                int wcPid = createProcess(wcProc, HIGH, FOREGROUND, 1, "wc", NULL);
                int pid = createProcessWithParams(pArr[i], LOW, BACKGROUND, 1, argv, 1, argc);
                connectProcesses(pid, wcPid);
                unblockProcess(wcPid);
                unblockProcess(pid);
                return wcPid;
            }
        }
        unknownProcess(argv[1]);
    }
    return -1;
}

int isVowel(char c){
    if(c >= 'A' && c <= 'Z') {
        c = c - 'A' + 'a';
    }
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

int filterUser(char* string){
    for(int i = 0;i < strlen(string); i ++){
        char c = string[i];
        if(!isVowel(c)){
            putChar(c);
        }
    }
    return 0;
}

int filterProc(ARGS) {
    int size = BUFFER_SIZE;
    char* buff = alloc(size);
    size_t read = getStrn(buff, size);
    while(buff[0] != -1 && read != 0) {
        for(int i=0; i<read; i++) {
            if(!isVowel(buff[i])) {
                putChar(buff[i]);
            }
        }
        getStrn(buff, size);
    }
    dealloc(buff);
    exitProc();
    return 0;
}

int filter(ARGS) {
    if(argc >= 2) {
        for(int i=0; pArr[i] != NULL; i++) {
            if(strcmp(pArr[i]->name, argv[1]) == 0){
                int filterPid = createProcess(filterProc, HIGH, FOREGROUND, 1, "filter", NULL);
                int pid = createProcessWithParams(pArr[i], LOW, BACKGROUND, 1, argv, 1, argc);
                connectProcesses(pid, filterPid);
                unblockProcess(filterPid);
                unblockProcess(pid);
                return filterPid;
            }
        }
        unknownProcess(argv[1]);
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

int phylo(ARGS) {
    initPhyloReunion();
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

int testMemory(ARGS) {
    test_mm(argc, argv);
    exitProc();
    return 0;
}

int playBeep(ARGS) {
    play_beep(2000, 100);
    exitProc();
    return 0;
}

int loudChild(ARGS) {
    while(1) {
        printFormat("\nLOUD CHILD ");
        setProcessPriority(getOwnPid(), HIGH);
        printFormat("SPEAKING");
    }
    exitProc();
    return 0;
}

int quietChild(ARGS) {
    postSem("quietChild");
    exitProc();
    return 0;
}

int monologue(ARGS) {
    int lc = createProcess(loudChild, HIGH, FOREGROUND, 1, "loudChild", NULL);
    int qc = createProcess(quietChild, LOW, BACKGROUND, 1, "quietChild", NULL);
    if(lc == -1 || qc == -1) {
        printFormat("\nProcess creation failed\n");
        exitProc();
    }
    openSem("quietChild", 0);
    unblockProcess(qc);
    unblockProcess(lc);
    waitSem("quietChild");
    destroySem("quietChild");
    killProcess(lc);
    printFormat("\nquiet child finally spoke");
    exitProc();
    return 0;
}

int stressTest(ARGS) {
    printFormat("\nStarting stress test");
    int testSize = 30;
    int bPid[testSize];
    for(int i=0; i<testSize && (i == 0 || bPid[i - 1] != -1); i++) {
        bPid[i] = createProcess(playBubbles, HIGH, BACKGROUND, 1, "stressTestBubbles", NULL);
        printFormat("\nCreated bubbles process with pid=%d", bPid[i]);
    }

    printFormat("\nProcess creation finished, waiting...");
    printFormat("\n\nProcess unblocking started\n");
    for(int i=0; i<testSize && bPid[i] != -1; i++) {
        unblockProcess(bPid[i]);
        printFormat("\nUnblocked process %d", bPid[i]);
    }
    printFormat("\n\nMax processes running\n");
    printFormat("\nProcess blocking started\n");
    for(int i=0; i<testSize && bPid[i] != -1; i++) {
        blockProcess(bPid[i]);
        printFormat("\nBlocked process %d", bPid[i]);
    }
    printFormat("\n\nAll processes blocked\nFINISHING\n");
    for(int i=0; i<testSize && bPid[i] != -1; i++) {
        killProcess(bPid[i]);
    }
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

int displayFIFOList(ARGS){
    displayFIFOs();
    return 0;
}

int connectProcsToFIFO(ARGS){
    connectToFIFO(argv[0],atoi(argv[1]),atoi(argv[2]));
    return 0;
}
