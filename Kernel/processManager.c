#include <lib.h>
#include <memoryManager.h>

#define RUNNING 0
#define READY 1
#define BLOCKED 2

#define MAX_PROC 2

uint64_t get_ip();
void load_process(uint64_t ip, uint8_t* stackTop, uint8_t* stackTrace);
uint64_t save_process();
uint8_t* get_stack_trace();

struct process {
    uint64_t ip;
    uint8_t* stackTop;
    uint8_t* stackTrace;
    char state;
} process;

typedef struct process* proc;

proc* processes;
int amount = 0;
int currProc = 0;

void initializeProcessManager() {
    processes = allocate(sizeof(process) * MAX_PROC);
}

void startProcess() {
    processes[amount]->ip = get_ip();
    processes[amount]->stackTop = allocate(sizeof(uint8_t) * 128);
    processes[amount]->stackTrace = processes[amount]->stackTop;
    amount = (amount + 1) % MAX_PROC;
}

void loadProcess(int pid) {
    load_process(processes[pid]->ip, processes[pid]->stackTop, processes[pid]->stackTrace);
}

void saveProcess(int pid) {
    processes[pid]->ip = save_process();
    processes[pid]->stackTrace = get_stack_trace();
}

void switchProcess() {
    currProc = (currProc + 1) % MAX_PROC;
    loadProcess(currProc);
}
