#include <lib.h>
#include <memoryManager.h>
#include <console.h>
#include <processManager.h>


struct process {
    char pname[20];
    int pid;
    uint8_t* stackTop;
    uint8_t* stackTrace;
    uint8_t state;
    int priority;
    int totalMemory;
} process;

static proc* processes;
static int amount = 0;
static int currProc = -1;
static int nextProc = -1;

uint8_t* get_ip();
uint8_t* prepare_process(uint8_t* stack, uint8_t* rip);

void initializeProcessManager() {
    processes = allocate(sizeof(process) * MAX_PROC);
    for(int i=0; i<MAX_PROC; i++) {
        processes[i]->state = DEAD;
    }
}

int startProcess(uint8_t* ip, int priority) {
    if(ip == NULL) {
        ip = get_ip();
    }

    int pid = amount;
    processes[pid]->stackTop = allocate(sizeof(uint8_t) * INIT_STACK_SIZE) + INIT_STACK_SIZE;
    processes[pid]->stackTrace = prepare_process(processes[pid]->stackTop, ip);
    processes[pid]->priority = priority;
    processes[pid]->state = READY;
    processes[pid]->totalMemory = INIT_STACK_SIZE;

    amount++;

    //TODO llamar al scheduler para que lo agregue a su lista
    return pid;
}

//TODO agregar manejo de excepciones ante la falla de memoria
void checkProcessHealth(int pid) {
    if(processes[pid]->stackTrace > processes[pid]->stackTop
    || processes[pid]->stackTrace < (processes[pid]->stackTop - processes[pid]->totalMemory)) {
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
    if(amount > 0 && currProc != nextProc && processes[nextProc]->state == READY) {
        processes[currProc]->stackTrace = (uint8_t*)rsp;
        processes[currProc]->state = READY;

        checkProcessHealth(nextProc);
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

void BeheadProcess(int pid){
    proc p=getProcess(pid);
    p->state=DEAD;
}
