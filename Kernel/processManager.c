#include <lib.h>
#include <memoryManager.h>
#include <console.h>
#include <processManager.h>
#include <process.h>
#include <scheduler.h>

struct process {
    char pname[PROC_NAME_LENGTH + 1];
    int pid;
    uint8_t* stackTop;
    uint8_t* stackTrace;
    uint8_t state;
    int priority;
    int totalMemory;
    uint8_t foreground;
} process;

proc processes[MAX_PROC] = {NULL};
static int amount = 0;
static int currProc = -1;
static int nextProc = -1;

//TODO checkear que pid es valido antes de hacer operaciones

uint8_t* get_ip();
uint8_t* prepare_process(uint8_t* stack, uint8_t* rip);
void interruptTick();
int findFirstAvailablePid();

void initializeProcessManager() {
    //this should be started at the very beginning, so pid=0 == sentinel
    startProcess(&processSentinel, LOW, FOREGROUND, "sentinel", 256);
}

int startProcess(void* ip, int priority, uint8_t foreground, char* name, unsigned int stackSize) {
    int pid = findFirstAvailablePid();

    if(pid == -1) {
        cPrint("couldn't find a PID");
        return -1;
    }
    if (ip == NULL) {
        ip = get_ip();
    }
    if (stackSize == 0) {
        stackSize = INIT_STACK_SIZE;
    }

    // Allocate memory for the new process
    processes[pid] = (struct process*)allocate(sizeof(struct process));

    if(processes[pid] == NULL) {
        return -1;
    }

    int i;
    for(i=0; i<PROC_NAME_LENGTH && name[i]!='\0'; i++) {
        processes[pid]->pname[i] = name[i];
    }

    processes[pid]->pname[i] = '\0';
    processes[pid]->pid = pid;
    processes[pid]->stackTop = allocate(stackSize) + stackSize;

    if(processes[pid]->stackTop == NULL) {
        deallocate(processes[pid]);
        return -1;
    }

    processes[pid]->stackTrace = prepare_process(processes[pid]->stackTop, ip);
    processes[pid]->priority = priority;
    processes[pid]->state = READY;
    processes[pid]->totalMemory = stackSize;
    processes[pid]->foreground = foreground;

    amount++;

    if(pid != 0) {
        addToScheduler(processes[pid]->pid);
    }
    interruptTick();
    return pid;
}

int checkProcessHealth(int pid) {
    if(pid == SENTINEL_PID) {
        //the sentinel process shouldn't be terminated
        return 0;
    }

    if(processes[pid]->stackTrace > processes[pid]->stackTop
       || processes[pid]->stackTrace < (processes[pid]->stackTop - processes[pid]->totalMemory)) {
        killProcess(pid);
        return 1;
    }

    //check if process needs more memory
    int usedMemory = ((uint64_t)processes[pid]->stackTop - (uint64_t)processes[pid]->stackTrace);
    if( usedMemory > ((processes[pid]->totalMemory / 4) * 3)) {
        int newTotalMemory = processes[pid]->totalMemory * 2;
        processes[pid]->stackTop = realloc(processes[pid]->stackTop - processes[pid]->totalMemory,
                                           newTotalMemory);
        if(processes[pid]->stackTop == NULL) {
            killProcess(pid);
            return -1;
        }
        processes[pid]->stackTop += newTotalMemory;
        processes[pid]->stackTrace = processes[pid]->stackTop - usedMemory;
        processes[pid]->totalMemory = newTotalMemory;
    }
    return 0;
}

void selectNextProcess(int pid) {
    nextProc = pid;
}

uint64_t switchProcess(uint64_t rsp) {
    if(amount > 0 && currProc != nextProc && processes[nextProc] != NULL && processes[nextProc]->state == READY) {
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

int getPriority(proc p) {
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

int getSpace() {
    for(int i=0;i<MAXPROCESSES;i++)
        if(processes[i]->state==DEAD)
            return i;
    return -1;
}

proc getProcess(int pid) {
    return processes[pid];
}

void setProcessForeground(int pid, int foreground) {
    processes[pid]->foreground = foreground;
}

void setProcessPriority(int pid, int priority) {
    processes[pid]->priority = priority;
    //TODO llamar al scheduler para avisarle del cambio
}

int getProcessPriority(int pid) {
    return processes[pid]->priority;
}

int isCurrentProcessInForeground() {
    return processes[currProc]->foreground == FOREGROUND || currProc == -1;
}

int isProcessAlive(int pid) {
    return processes[pid] != NULL;
}

void killProcess(int pid) {
    if(processes[pid] != NULL) {
        int priority = processes[pid]->priority;
        deallocate(processes[pid]->stackTop - processes[pid]->totalMemory);
        deallocate(processes[pid]);
        processes[pid] = NULL;
        removeFromScheduler(pid, priority);
        amount--;
        if(pid == currProc) {
            interruptTick();
        }
    }
}

int findFirstAvailablePid() {
    if(amount == MAX_PROC) {
        return -1;
    }

    for(int i=0; i<MAX_PROC; i++) {
        if(processes[i] == NULL) {
            return i;
        }
    }

    return -1;
}
