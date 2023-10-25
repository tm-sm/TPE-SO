#include <scheduler.h>
#include <processManager.h>
#include <stdio.h>
#include <console.h>


typedef struct node{
    int pid;
    struct node * next;
}node;

void roundRobin();


static node * procs[3] = {NULL};
static node * procsLast[3] = {NULL};

static node * runningProc = NULL;

void scheduler() {
    roundRobin();
}

void roundRobin() {
    node * aux = NULL;
    node * toRun = NULL;
    if(runningProc == NULL) {
        return;
    }
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
                cPrint("\n");
                cPrintDec(runningProc->pid);
                selectNextProcess(runningProc->pid);
            }
            while (aux != toRun) {
                if (getStateFromPid(toRun->pid) == READY) {
                    runningProc = toRun;
                    cPrint("\n");
                    cPrintDec(runningProc->pid);
                    selectNextProcess(runningProc->pid);
                    return;
                }
                toRun = toRun->next;
            }
        }
    }

    /*
    if(getStateFromPid(runningProc->pid) != RUNNING) {
        selectNextProcess(SENTINEL_PID);
    }
     */
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
    addNodeToPriority(new, priority);
}

