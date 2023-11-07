#include <scheduler.h>
#include <processManager.h>
#include <stdio.h>
#include <console.h>
#include <sems.h>

#define MAX_TICKS 3
#define MUTEX "schedulerMutex"

typedef struct Node{
    int pid;
    struct Node * next;
    int ticks;
}Node;

void roundRobin();
void lowerPriority(Node* Node);


static Node * procs[3] = {NULL};
static Node * procsLast[3] = {NULL};

static Node * runningProc = NULL;
static Node sentinel = {0, NULL, 0};

void initializeScheduler() {
    openSem(MUTEX, 1);
}

void scheduler() {
    roundRobin();
}

void roundRobin() {
    Node * aux = NULL;
    Node * toRun = NULL;

    if(runningProc == NULL) {
        return;
    }

    if(runningProc->pid != SENTINEL_PID) {
        runningProc->ticks++;
        if (runningProc->ticks >= MAX_TICKS) {
            runningProc->ticks = 0;
            lowerPriority(runningProc);
        }
    }

    for(int i=0; i<3; i++) {

        if (getPriorityFromPid(runningProc->pid) == i) {
            aux = runningProc;
            toRun = runningProc->next;
        } else if(procs[i] != NULL) {
            toRun = procs[i];
            aux = procs[i];
        }

        if(aux != NULL && toRun != NULL) {

            if(getStateFromPid(toRun->pid) == READY) {
                runningProc = toRun;
                selectNextProcess(runningProc->pid);
                return;
            }
            toRun = toRun->next;

            while(aux != toRun) {
                if (getStateFromPid(toRun->pid) == READY) {
                    runningProc = toRun;
                    selectNextProcess(runningProc->pid);
                    return;
                }
                toRun = toRun->next;
            }
        }
    }

    if(getStateFromPid(runningProc->pid) != RUNNING) {
        runningProc = &sentinel;
        selectNextProcess(SENTINEL_PID);
    }
}

int getRunningPid(){
    return runningProc->pid;
}

void addNodeToPriority(Node* n, int priority) {
    waitSem(MUTEX);
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
    postSem(MUTEX);
}
void removeNodefromPriority(Node* n, int p){
    Node* aux = procs[p];
    while(aux->next != n)
        aux = aux->next;
    aux->next = n->next;
    return ;
}

void lowerPriority(Node* Node) {
    waitSem(MUTEX);
    int priority = getPriorityFromPid(Node->pid);
    Node->ticks = 0;
    if(priority == LOW) {
        postSem(MUTEX);
        return;
    }
    postSem(MUTEX);
    setProcessPriority(Node->pid, priority + 1);
}

void changeProcessPriority(int pid, int originalPriority, int newPriority) {
    waitSem(MUTEX);
    Node* curr = procs[originalPriority];
    Node* prev = procsLast[originalPriority];

    if(curr == prev && curr->pid == pid) {
        procs[originalPriority] = NULL;
        procsLast[originalPriority] = NULL;
    } else if(curr->pid == pid) {
        procsLast[originalPriority]->next = curr->next;
        procs[originalPriority] = curr->next;
    } else {

        while((curr->pid != pid) && (curr != procsLast[originalPriority])) {
            prev = curr;
            curr = curr->next;
        }

        if(curr->pid != pid) {
            //finished the whole list and didn't find anything
            postSem(MUTEX);
            return;
        } else if(curr == procsLast[originalPriority]) {
            procsLast[originalPriority] = prev;
            prev->next = curr->next;
            postSem(MUTEX);
            return;
        } else {
            prev->next = curr->next;
        }
    }
    postSem(MUTEX);
    addNodeToPriority(curr, newPriority);
}

void addToScheduler(int pid) {
    waitSem(MUTEX);
    int priority = getPriorityFromPid(pid);
    Node * new = allocate(sizeof(Node));
    new->pid = pid;
    new->ticks = 0;
    addNodeToPriority(new, priority);
    postSem(MUTEX);
}

void removeFromScheduler(int pid, int priority) {
    waitSem(MUTEX);
    if(pid == procs[priority]->pid && procs[priority] == procsLast[priority]) {
        deallocate(procs[priority]);
        procs[priority] = procsLast[priority] = NULL;
        postSem(MUTEX);
        return;
    }
    Node *prev = procs[priority];
    Node *n = procs[priority]->next;
    while(n->pid != pid) {
        if(n == procsLast[priority]) {
            postSem(MUTEX);
            return;
        }
        prev = n;
        n = n->next;
    }
    prev->next = n->next;
    procsLast[priority] = prev;
    procs[priority] = prev->next;
    deallocate(n);
    postSem(MUTEX);
}

void printPriorityList(int priority) {
    if(procs[priority] == NULL) {
        return;
    }
    Node *n = procs[priority];
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

