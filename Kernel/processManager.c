#include <lib.h>
#include <memoryManager.h>
#include <console.h>
#include <processManager.h>
#include <process.h>
#include <scheduler.h>
#include <stddef.h>
#include <fdManager.h>
#include <time.h>
#include <utils.h>
#include <interrupts.h>
#include <keyboardDriver.h>
#include <sems.h>

#define MUTEX "processManagerMutex"

typedef struct ChildNode* cNode;

#define TRUE 1
#define FALSE 0
#define STDOUT 1
#define STDIN 0

struct ChildNode {
    int pid;
    cNode next;
}ChildNode;

struct Process {
    char pname[PROC_NAME_LENGTH + 1];
    int pid;
    uint8_t* stackTop;
    uint8_t* stackTrace;
    uint8_t state;
    int priority;
    int totalMemory;
    int stdin;
    int stdout;
    char** argv;
    int argc;
    int parentPid;
    char waitingForChildren;
    int waitingForChild;
    cNode children;
} Process;

proc processes[MAX_PROC] = {NULL};
static int amount = 0;
static int currProc = -1;
static int nextProc = -1;


uint8_t* get_ip();
uint8_t* prepare_process(uint8_t* stack, uint8_t* rip, int argc, char* argv[]);
void interruptTick();
int findFirstAvailablePid();
int isPidValid(int pid);
int addChildNode(int parentPid, int childPid);
void removeChildNode(int parentPid, int childPid);

int fgStack[MAX_PROC];
int lastFgProc = -1;

void initializeProcessManager() {
    //this should be started at the very beginning, so pid=0 == sentinel
    openSem(MUTEX, 1);
    startProcess(&processSentinel, UNDEFINED, FOREGROUND, 0, "sentinel", 1024, NULL);
}

int startProcess(void* ip, int priority, int foreground, int isBlocked, const char* name, unsigned int stackSize, char* argv[]) {
    _cli();
    int pid = findFirstAvailablePid();

    int argc = 0;
    if(argv != NULL) {
        while(argv[argc] != NULL) {
            argc++;
        }
    }

    if(pid == -1) {
        for(int j=0; j<argc; j++) {
            deallocate(argv[j]);
        }
        deallocate(argv);
        _sti();
        return -1;
    }
    if(ip == NULL) {
        ip = get_ip();
    }
    if(stackSize == 0) {
        stackSize = INIT_STACK_SIZE;
    }

    processes[pid] = (struct Process*)allocate(sizeof(struct Process));

    if(processes[pid] == NULL) {
        for(int j=0; j<argc; j++) {
            deallocate(argv[j]);
        }
        deallocate(argv);
        _sti();
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
        for(int j=0; j<argc; j++) {
            deallocate(argv[j]);
        }
        deallocate(argv);
        deallocate(processes[pid]);
        _sti();
        return -1;
    }

    processes[pid]->stackTrace = prepare_process(processes[pid]->stackTop, ip, argc, argv);
    processes[pid]->priority = priority;
    processes[pid]->state = (isBlocked) ? (BLOCKED) : (READY);
    processes[pid]->totalMemory = (int)stackSize;
    processes[pid]->argv = argv;
    processes[pid]->argc = argc;
    processes[pid]->children = NULL;
    processes[pid]->waitingForChildren = FALSE;
    processes[pid]->waitingForChild = FALSE;

    processes[pid]->stdin = STDIN;
    processes[pid]->stdout = STDOUT;

    processes[pid]->parentPid = currProc;

    if(processes[pid]->parentPid >= 1) {
        //the sentinel has no children
        if(addChildNode(processes[pid]->parentPid, pid) == -1) {
            for(int j=0; j<argc; j++) {
                deallocate(argv[j]);
            }
            deallocate(argv);
            deallocate(processes[pid]->stackTop - processes[pid]->totalMemory);
            deallocate(processes[pid]);
            _sti();
            return -1;
        }
    }

    amount++;

    if(pid != 0) {
        addToScheduler(processes[pid]->pid);
    }

    if(foreground) {
        addToFgStack(pid);
    }

    _sti();
    interruptTick();
    return pid;
}

void selectNextProcess(int pid) {
    waitSem(MUTEX);
    if(isPidValid(pid)) {
        nextProc = pid;
    }
    postSem(MUTEX);
}

uint64_t switchProcess(uint64_t rsp) {
    waitSem(MUTEX);
    if(amount > 0 && currProc != nextProc && processes[nextProc] != NULL && processes[nextProc]->state == READY) {
        if(currProc != -1) {
            processes[currProc]->stackTrace = (uint8_t *) rsp;
            if(processes[currProc]->state == RUNNING) {
                processes[currProc]->state = READY;
            }
        }
        currProc = nextProc;
        processes[currProc]->state = RUNNING;
        postSem(MUTEX);
        return (uint64_t)processes[currProc]->stackTrace;
    }
    postSem(MUTEX);
    return 0;
}

int getPriority(proc p) {
    return p->priority;
}

int getPriorityFromPid(int pid) {
    if(!isPidValid(pid) || pid == 0) {
        return UNDEFINED;
    }
    return processes[pid]->priority;
}

int getStateFromPid(int pid) {
    if(!isPidValid(pid)) {
        return DEAD;
    }
    return processes[pid]->state;
}

int getPid(proc p) {
    return p->pid;
}

int getSpace() {
    return (MAX_PROC - 1) - amount;
}

proc getProcess(int pid) {
    waitSem(MUTEX);
    if(isPidValid(pid)) {
        postSem(MUTEX);
        return processes[pid];
    }
    postSem(MUTEX);
    return NULL;
}
void yieldProcess(){
    interruptTick();
}

void setProcessForeground(int pid, int foreground) {
    waitSem(MUTEX);
    if(isPidValid(pid)) {
        switch (foreground) {
            case FOREGROUND:
                //if already in stack, move it to the front
                removeFromFgStack(pid);
                addToFgStack(pid);
                break;
            case BACKGROUND:
                removeFromFgStack(pid);
                break;
            default:
                break;
        }
    }
    postSem(MUTEX);
}

void setProcessPriority(int pid, int priority) {
    waitSem(MUTEX);
    if(isPidValid(pid) && (priority == LOW || priority == MED || priority == HIGH)) {
        int oldPriority = processes[pid]->priority;
        processes[pid]->priority = priority;
        changeProcessPriority(pid, oldPriority, priority);
    }
    postSem(MUTEX);
}

int getProcessPriority(int pid) {
    waitSem(MUTEX);
    int priority = UNDEFINED;
    if(isPidValid(pid)) {
        priority = processes[pid]->priority;
    }
    postSem(MUTEX);
    return priority;
}

int isProcessInForeground(int pid) {
    //if lastFgProc == -1, no process currently exists
    //meaning whatever is running should have access to the console/keyboard
    return lastFgProc == -1 || pid == fgStack[lastFgProc];
}

int isCurrentProcessInForeground() {
    return isProcessInForeground(currProc);
}

void addToFgStack(int pid) {
    waitSem(MUTEX);
    if(isPidValid(pid)) {
        if (lastFgProc < MAX_PROC) {
            lastFgProc++;
            fgStack[lastFgProc] = pid;
        }
    }
    postSem(MUTEX);
}

// connects stdout proc1 to stdin proc2
int connectProcs(int pidProc1, int pidProc2) {
    proc proc1 = getProcess(pidProc1);
    proc proc2 = getProcess(pidProc2);

    int fd[2];

    if(customPipe(fd) == -1) {
        return -1;
    }

    closeFD(proc1->stdout);
    closeFD(proc2->stdin);

    proc1->stdout = fd[0];
    proc2->stdin = fd[1];

    return 0;
}

//redirects stdout of proc 1 to stdin of named pipe and stdout of named pipe to stdin proc 2
int connectToNamedPipe(const char* name, int pidProc1, int pidProc2) {
    proc proc1 = getProcess(pidProc1);
    proc proc2 = getProcess(pidProc2);

    closeFD(proc1->stdout);
    closeFD(proc2->stdin);

    return setToNamedPipeFd(&proc1->stdout, &proc2->stdin, name);
}

void removeFromFgStack(int pid) {
    //pid is not checked, as processes are removed from the stack after they are killed
    waitSem(MUTEX);
    if(lastFgProc == -1) {
        postSem(MUTEX);
        return;
    }
    if(fgStack[lastFgProc] == pid) {
        lastFgProc--;
    } else {
        for(int i = 0; i < lastFgProc; i++) {
            if(fgStack[i] == pid) {
                memcpy(fgStack + i, fgStack + i + 1, sizeof(int) * (MAX_PROC - i - 1));
                lastFgProc--;
            }
        }
    }
    postSem(MUTEX);
}

int isProcessAlive(int pid) {
    waitSem(MUTEX);
    int ret = 0;
    if(isPidValid(pid)) {
        ret = processes[pid] != NULL;
    }
    postSem(MUTEX);
    return ret;
}

void killProcess(int pid) {
    _cli(); //blocks interruptions to prevent a process flagged for being killed being ran

    if(pid == 0) {
        //the sentinel shouldn't be terminated
        _sti();
        return;
    }

    if(isPidValid(pid)) {
        int priority = processes[pid]->priority;
        int parentPid = processes[pid]->parentPid;

        if(lastFgProc >= 0 && fgStack[lastFgProc] == pid && doubleBufferingEnabled()) {
            forceDisableDoubleBuffering();
            forceClearScreen();
        }

        //because each children removes itself from the parent, c always points to the next child
        for(cNode c = processes[pid]->children; c != NULL; c = processes[pid]->children) {
            killProcess(c->pid);
        }
        _cli();
        removeChildNode(processes[pid]->parentPid, pid);

        char aux[2] = {EOF, '\0'};
        write(processes[pid]->stdout, aux, 2);

        closeFD(processes[pid]->stdin);
        closeFD(processes[pid]->stdout);

        if(processes[pid]->argv != NULL) {
            for(int i=0; i<processes[pid]->argc; i++) {
                deallocate(processes[pid]->argv[i]);
            }
            deallocate(processes[pid]->argv);
        }

        deallocate(processes[pid]->stackTop - processes[pid]->totalMemory);
        deallocate(processes[pid]);
        processes[pid] = NULL;
        removeFromFgStack(pid);
        removeFromScheduler(pid, priority);
        amount--;
        deallocateAllProcessRelatedMem(pid);
        notifyParent(parentPid, pid);
        if(pid == currProc) {
            _sti();
            interruptTick();
        }
    }
    _sti();
}

int findFirstAvailablePid() {
    //no mutex as it is only called in startProcess (which already has a mutex)
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

int getActiveProcessPid() {
    int ret = currProc;
    return ret;
}

void listAllProcesses() {
    for(int i=0; i<MAX_PROC; i++) {
        if(processes[i] != NULL) {
            cPrint("\n");
            cPrintDec(i);
            cPrint(" | ");
            cPrint(processes[i]->pname);
            switch(processes[i]->priority) {
                case HIGH:
                    cPrint(" | HIGH PRIO");
                    break;
                case MED:
                    cPrint(" | MED PRIO");
                    break;
                case LOW:
                    cPrint(" | LOW PRIO");
                    break;
                default:
                    cPrint(" | N/A");
            }
            (fgStack[lastFgProc] == processes[i]->pid) ? (cPrint(" | FOREGROUND")) : (cPrint(" | BACKGROUND"));
            cPrint(" | rsp: ");
            cPrintHex((uint64_t)processes[i]->stackTrace);
            cPrint(" | rbp: ");
            cPrintHex((uint64_t)processes[i]->stackTop);
            cPrint(" |");
            if(processes[i]->pid != SENTINEL_PID) {
                if(processes[i]->state == BLOCKED) {
                    cPrint(" BLOCKED");
                } else {
                    cPrint(" UNBLOCKED");
                }
            } else {
                cPrint(" DORMANT");
            }
        }
    }
}

int isPidValid(int pid) {
    int ret = pid < MAX_PROC && pid >= 0 && processes[pid] != NULL;
    return ret;
}

void unblockProcess(int pid) {
    waitSem(MUTEX);
    if(isPidValid(pid)) {
        processes[pid]->state = READY;
    }
    postSem(MUTEX);
}

void blockProcess(int pid) {
    if(pid == 0) {
        return;
    }
    waitSem(MUTEX);
    if(isPidValid(pid)) {
        processes[pid]->state = BLOCKED;
        postSem(MUTEX);
        interruptTick();
    }
}

void blockCurrentProcess() {
    blockProcess(currProc);
}

int getForegroundPid() {
    waitSem(MUTEX);
    if(lastFgProc != -1) {
        postSem(MUTEX);
        return fgStack[lastFgProc];
    }
    postSem(MUTEX);
    return -1;
}

void killProcessInForeground() {
    killProcess(fgStack[lastFgProc]);
}

//only called within procManager, no checks are performed
int addChildNode(int parentPid, int childPid) {
    //no mutex, read findFistAvailablePid
    cNode child = allocate(sizeof(ChildNode));
    if(child == NULL) {
        return -1;
    }
    child->pid = childPid;
    child->next = processes[parentPid]->children;
    processes[parentPid]->children = child;
    return 0;
}

void removeChildNode(int parentPid, int childPid) {
    waitSem(MUTEX);
    if(parentPid == -1 || parentPid == 0) {
        postSem(MUTEX);
        return;
    }
    cNode curr;
    cNode prev;
    curr = processes[parentPid]->children;
    if(curr->pid == childPid) {
        processes[parentPid]->children = curr->next;
        postSem(MUTEX);
        return;
    }
    while(curr != NULL && curr->pid != childPid) {
        prev = curr;
        curr = curr->next;
    }
    if(curr == NULL) {
        postSem(MUTEX);
        return;
    }
    prev->next = curr->next;
    deallocate(curr);
    postSem(MUTEX);
}

void waitForChild(int pid) {
    waitSem(MUTEX);
    if(!isPidValid(currProc) || pid == 0) {
        postSem(MUTEX);
        return;
    }
    //moves the child process to the front of the list,
    //every time it's notified it checks if the first position's pid == pid
    processes[currProc]->waitingForChildren = FALSE;

    cNode curr;
    cNode prev;
    curr = processes[currProc]->children;

    if(curr->pid == pid) {
        processes[currProc]->waitingForChild = pid;
    } else {
        prev = curr;
        curr = curr->next;
        while((curr != NULL && curr->pid != pid)) {
            curr = curr->next;
        }
        if(curr == NULL) {
            postSem(MUTEX);
            return;
        }
        processes[currProc]->waitingForChild = pid;
        prev->next = curr->next;
        curr->next = processes[currProc]->children;
        processes[currProc]->children = curr;
    }

    while(processes[currProc]->children->pid == pid) {
        postSem(MUTEX);
        blockCurrentProcess();
        waitSem(MUTEX);
    }
    processes[currProc]->waitingForChild = FALSE;
    postSem(MUTEX);
}

void waitForChildren() {
    waitSem(MUTEX);
    if(isPidValid(currProc)) {
        processes[currProc]->waitingForChildren = TRUE;
        processes[currProc]->waitingForChild = FALSE;
        while(processes[currProc]->children != NULL) {
            postSem(MUTEX);
            blockCurrentProcess();
            waitSem(MUTEX);
        }
        processes[currProc]->waitingForChildren = FALSE;
    }
    postSem(MUTEX);
}

void notifyParent(int parentPid, int childPid) {
    waitSem(MUTEX);
    if(isPidValid(parentPid)) {
        if(processes[parentPid]->waitingForChildren
           || processes[parentPid]->waitingForChild == childPid) {
            postSem(MUTEX);
            removeChildNode(parentPid, childPid);
            unblockProcess(parentPid);
        }
    }
    postSem(MUTEX);
}

int getStdinFd(int pid) {
    if(isPidValid(pid)) {
        return processes[pid]->stdin;
    }
    return STDIN;
}

int getStdoutFd(int pid) {
    if(isPidValid(pid)) {
        return processes[pid]->stdout;
    }
    return STDOUT;
}

void exitProc() {
    killProcess(currProc);
}
