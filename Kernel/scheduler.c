#include <scheduler.h>
#include <processManager.h>
#include <stdio.h>
#include <console.h>


typedef struct node{
    int pid;
    struct node * next;
    int counter;
}node;

void roundRobin();
void dumbSchedule();


static node * procs[3] = {NULL};
static node * procsLast[3] = {NULL};

static node * runningProc = NULL;

void scheduler() {
    roundRobin();
}

void dumbSchedule() {
    //TODO sacarlo para la entrega, es mas debug
    //for testing purposes
    if(runningProc != NULL) {
        runningProc = runningProc->next;
        selectNextProcess(runningProc->pid);
    }
}

void roundRobin() {
    node * aux = NULL;
    node * toRun = NULL;
    if(runningProc == NULL) {
        return;
    }
    if(getPriorityFromPid(runningProc->pid) == HIGH && runningProc->counter < 3){
        runningProc->counter ++;
    }else if(getPriorityFromPid(runningProc->pid) == MED && runningProc->counter < 2){
        runningProc->counter ++;
    }else{
        runningProc->counter = 0;
        for(int i = 0; i < 3;i++){
            if(getPriorityFromPid(runningProc->pid) == i){
                aux = runningProc;
                toRun = runningProc->next;
            } else if(procs[i] != NULL){
            toRun = procs[i]->next;
            aux = procs[i];
            }
            if(aux != NULL && toRun != NULL) {
            if (aux == toRun && getStateFromPid(toRun->pid) == READY) {
                runningProc = toRun;
                selectNextProcess(runningProc->pid);
                return;
                }
                while (aux != toRun) {
                    if (getStateFromPid(toRun->pid) == READY) {
                        runningProc = toRun;
                        selectNextProcess(runningProc->pid);
                        return;
                    }
                    toRun = toRun->next;
                }
            }
        }
    }

    if(getStateFromPid(runningProc->pid) != RUNNING) {
        selectNextProcess(SENTINEL_PID);
    }
}



void addNodeToPriority(node * n, int priority) {
    if(procs[priority] == NULL) {
        procs[priority] = n;
        procsLast[priority] = n;
        n->next = procsLast[priority];
    } else {
        procsLast[priority]->next = n;
        n->next = procs[priority];
        procsLast[priority] = n;
    }

    if(runningProc == NULL) {
        runningProc = n;
    }
}

void addToScheduler(int pid) {
    int priority = getPriorityFromPid(pid);
    node * new = allocate(sizeof(node));
    new->pid = pid;
    new->counter = 0;
    addNodeToPriority(new, priority);
}

void removeFromScheduler(int pid, int priority) {
    if(pid == procs[priority]->pid && procs[priority] == procsLast[priority]) {
        deallocate(procs[priority]);
        procs[priority] = procsLast[priority] = NULL;
        return;
    }
    node *prev = procs[priority];
    node *n = procs[priority]->next;
    while(n->pid != pid) {
        if(n == procsLast[priority]) {
            return;
        }
        prev = n;
        n = n->next;
    }
    prev->next = n->next;
    procsLast[priority] = prev;
    procs[priority] = prev->next;
    deallocate(n);
}

void printPriorityList(int priority) {
    if(procs[priority] == NULL) {
        return;
    }
    node *n = procs[priority];
    cPrint("\n");
    cPrintDec(n->pid);
    cPrint(" -> ");
    cPrintDec(n->next->pid);
    n = n->next;
    while(n != procsLast[priority]) {
        cPrintDec(n->pid);
        cPrint(" -> ");
        cPrintDec(n->next->pid);
        n = n->next;
    }
}

