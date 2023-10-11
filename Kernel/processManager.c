#include <lib.h>
#include <memoryManager.h>
#include <console.h>

#define REGISTERS 16

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3

#define MAX_PROC 2

struct process {
    uint8_t* ip;
    uint8_t* stackTop;
    uint8_t* stackTrace;
    uint64_t stack[REGISTERS];
    uint8_t state;
    int priority;
} process;

typedef struct process* proc;

static proc* processes;
static int amount = 0;
static int currProc = 1;

uint8_t* get_ip();
uint64_t switch_process(proc nextProcess);

void initializeProcessManager() {
    processes = allocate(sizeof(process) * MAX_PROC);
    for(int i=0; i<MAX_PROC; i++) {
        processes[i]->state = DEAD;
    }
}

void startProcess() {
    processes[amount]->ip = get_ip();
    processes[amount]->stackTop = allocate(sizeof(uint8_t) * 128);
    processes[amount]->stackTrace = processes[amount]->stackTop;
    for(int i=0; i<REGISTERS; i++) {
        processes[amount]->stack[i] = 0;
    }
    processes[amount]->state = READY;
    amount = (amount + 1) % MAX_PROC;
}

void switchProcess(uint64_t* registers, int pid) {
    if(processes[pid]->state == READY) {
        //TODO activarlo cuando ya haya multiplos procesos corriendo
        /*processes[currProc]->state = READY;
        for(int i=0; i<REGISTERS; i++) {
            processes[currProc]->stack[i] = registers[i];
        }
        processes[currProc]->stackTrace = (uint8_t*)&registers[16];
        processes[currProc]->ip = (uint8_t*)&registers[17];
         */
        processes[pid]->state = RUNNING;
        switch_process(processes[pid]);
    }
    return;
}


int getPriority(proc p){
    return p->priority;
}
