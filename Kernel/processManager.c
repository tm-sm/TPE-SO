#include <lib.h>
#include <memoryManager.h>
#include <console.h>

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3
#define INIT_STACK_SIZE 1024

#define MAX_PROC 10

struct process {
    uint8_t* stackTop;
    uint8_t* stackTrace;
    uint8_t state;
    int priority;
} process;

typedef struct process* proc;

static proc* processes;
static int amount = 0;
static int currProc = -1;
static int nextProc = 0;

uint8_t* get_ip();
uint8_t* prepare_process(uint8_t* stackPtr, uint8_t* rbp, uint8_t* rip);

void initializeProcessManager() {
    processes = allocate(sizeof(process) * MAX_PROC);
    for(int i=0; i<MAX_PROC; i++) {
        processes[i]->state = DEAD;
    }
}

int startProcess(uint8_t* ip) {
    int pid = amount;
    processes[pid]->stackTop = allocate(sizeof(uint8_t) * INIT_STACK_SIZE) + INIT_STACK_SIZE;

    if(ip == NULL) {
        ip = get_ip();
    }
    processes[pid]->stackTrace = prepare_process(processes[pid]->stackTop,
                                                    processes[pid]->stackTop, ip);
    processes[pid]->state = READY;

    amount++;
    return pid;
}

void selectNextProcess(int pid) {
    nextProc = pid;
}

uint64_t switchProcess(uint64_t rsp) {
    if(amount > 0 && currProc != nextProc && processes[nextProc]->state == READY) {
        processes[currProc]->stackTrace = (uint8_t*)rsp;
        processes[currProc]->state = READY;
        processes[nextProc]->state = RUNNING;
        uint64_t returnVal = (uint64_t)processes[nextProc]->stackTrace;
        currProc = nextProc;
        return returnVal;
    }
    return 0;
}


int getPriority(proc p){
    return p->priority;
}
