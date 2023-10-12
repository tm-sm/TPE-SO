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
} process;



static proc* processes;
static int amount = 0;
static int currProc = -1;
static int nextProc = -1;

uint8_t* get_ip();
uint8_t* prepare_process(uint8_t* stackPtr, uint8_t* rbp, uint8_t* rip);

void initializeProcessManager() {
    processes = allocate(sizeof(process) * MAX_PROC);
    for(int i=0; i<MAX_PROC; i++) {
        processes[i]->state = DEAD;
    }
}

int startProcess(uint8_t* ip, int priority) {
    int pid = amount;
    processes[pid]->stackTop = allocate(sizeof(uint8_t) * INIT_STACK_SIZE) + INIT_STACK_SIZE;

    if(ip == NULL) {
        ip = get_ip();
    }
    processes[pid]->stackTrace = prepare_process(processes[pid]->stackTop,
                                                    processes[pid]->stackTop, ip);

    processes[pid]->priority = priority;
    processes[pid]->state = READY;

    amount++;

    //TODO llamar al scheduler para que lo agregue a su lista
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
