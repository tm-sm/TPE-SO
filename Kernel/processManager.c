#include <lib.h>
#include <memoryManager.h>
#include <console.h>
#include <processManager.h>
#include <process.h>
#include <scheduler.h>

struct process {
    char pname[20];
    int pid;
    uint8_t* stackTop;
    uint8_t* stackTrace;
    uint8_t state;
    int priority;
    int totalMemory;
    uint8_t foreground;
} process;

proc processes[MAX_PROC];
static int amount = 0;
static int currProc = -1;
static int nextProc = -1;
static int fgProc = -1;

uint8_t* get_ip();
uint8_t* prepare_process(uint8_t* stack, uint8_t* rip);
void startSentinel();
void interruptNop();

void initializeProcessManager() {
    for(int i=0; i<MAX_PROC; i++) {
        processes[i]->state = DEAD;
    }

    startSentinel();
}

void startSentinel() {
    void* ip = &processSentinel;
    int pid = SENTINEL_PID;
    processes[pid]->pid = pid;
    processes[pid]->stackTop = allocate(sizeof(uint8_t) * SENTINEL_STACK_SIZE) + SENTINEL_STACK_SIZE;
    processes[pid]->stackTrace = prepare_process(processes[pid]->stackTop, ip);
    processes[pid]->priority = LOW;
    processes[pid]->state = READY;
    processes[pid]->totalMemory = SENTINEL_STACK_SIZE;
    processes[pid]->totalMemory = BACKGROUND;

    amount++;
}

int startProcess(void* ip, int priority, uint8_t foreground) {
    if (ip == NULL) {
        ip = get_ip();
    }

    int pid = amount;

    // Allocate memory for the new process
    processes[pid] = (struct process*)allocate(sizeof(struct process));

    processes[pid]->pid = pid;
    processes[pid]->stackTop = allocate(INIT_STACK_SIZE) + INIT_STACK_SIZE;
    processes[pid]->stackTrace = prepare_process(processes[pid]->stackTop, ip);
    processes[pid]->priority = priority;
    processes[pid]->state = READY;
    processes[pid]->totalMemory = INIT_STACK_SIZE;
    processes[pid]->foreground = foreground;

    amount++;

    addToScheduler(processes[pid]->pid);
    scheduler();
    interruptNop();
    return pid;
}

//TODO agregar manejo de excepciones ante la falla de memoria
void checkProcessHealth(int pid) {
    if(pid == SENTINEL_PID) return;

    if(processes[pid]->stackTrace > processes[pid]->stackTop
       || processes[pid]->stackTrace < (processes[pid]->stackTop - processes[pid]->totalMemory)) {
        return;
        //TODO matar al proceso por escribir en memoria invalida
    }

    //check if process needs more memory
    int usedMemory = ((uint64_t)processes[pid]->stackTop - (uint64_t)processes[pid]->stackTrace);
    if( usedMemory > ((processes[pid]->totalMemory / 4) * 3)) {
        int newTotalMemory = processes[pid]->totalMemory * 2;
        processes[pid]->stackTop = realloc(processes[pid]->stackTop - processes[pid]->totalMemory,
                                           newTotalMemory);
        processes[pid]->stackTop += newTotalMemory;
        processes[pid]->stackTrace = processes[pid]->stackTop - usedMemory;
        processes[pid]->totalMemory = newTotalMemory;
    }
}

void selectNextProcess(int pid) {
    nextProc = pid;
}

uint64_t switchProcess(uint64_t rsp) {
    /*if(currProc != -1) {
        cPrint("\nCurr: ");
        cPrintDec(currProc);
        cPrint("Next: ");
        cPrintDec(nextProc);
    }*/
    if(amount > 0 && currProc != nextProc) {
        if(currProc != -1) {
            processes[currProc]->stackTrace = (uint8_t *) rsp;
            processes[currProc]->state = READY;
        }
        currProc = nextProc;
        checkProcessHealth(currProc);
        processes[currProc]->state = RUNNING;
        return (uint64_t)processes[currProc]->stackTrace;
    }
    return 0;
}

int getPriority(proc p){
    return p->priority;
}

int getPriorityFromPid(int pid) {
    return processes[pid]->priority;
}

int getStateFromPid(int pid) {
    return processes[pid]->state;
}

int getPid(proc p) {
    return p->pid;
}

int getSpace(){
    for(int i=0;i<MAXPROCESSES;i++)
        if(processes[i]->state==DEAD)
            return i;
    return -1;
}

proc getProcess(int pid){
    for(int i=0;i<MAXPROCESSES;i++) {
        if (processes[i]->pid == pid)
            return processes[i];
    }
    return NULL;
}

void setProcessPriority(int pid, int priority) {
    processes[pid]->priority = priority;
}

int getProcessPriority(int pid) {
    return processes[pid]->priority;
}

int isCurrentProcessInForeground() {
    return processes[currProc]->foreground == FOREGROUND;
}

void BeheadProcess(int pid){
    proc p=getProcess(pid);
    p->state=DEAD;
}
