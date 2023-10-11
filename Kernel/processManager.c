#include <lib.h>
#include <memoryManager.h>
#include <console.h>

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3

#define MAX_PROC 2

struct process {
    uint64_t ip;
    uint8_t* stackTop;
    uint8_t* stackTrace;
    char state;
} process;

typedef struct process* proc;

static proc* processes;
static int amount = 0;
static int currProc = 1;

uint64_t get_ip();
void load_process(uint64_t ip, uint8_t* stackTop, uint8_t* stackTrace);
uint64_t switch_process(uint64_t* registers, proc prevProcess, proc nextProcess);
uint8_t* get_stack_trace();

void initializeProcessManager() {
    processes = allocate(sizeof(process) * MAX_PROC);
    for(int i=0; i<MAX_PROC; i++) {
        processes[i]->state = DEAD;
    }
}

void startProcess() {
    processes[amount]->ip = 0x400000;
    processes[amount]->stackTop = allocate(sizeof(uint8_t) * 128);
    for(int i=0; i<17; i++) {
        processes[amount]->stackTop[i] = 0;
    }
    processes[amount]->stackTrace = processes[amount]->stackTop + 17;
    processes[amount]->state = READY;
    amount = (amount + 1) % MAX_PROC;
}

void switchProcess(uint64_t* registers, int pid) {
    if(processes[pid]->state == READY) {
        processes[currProc]->state = READY;
        processes[pid]->state = RUNNING;
        cPrintHex(switch_process(registers, processes[currProc], processes[pid]));
    }
    return;
}
