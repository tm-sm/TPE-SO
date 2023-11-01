#include <scheduler.h>
#include <processManager.h>
#include <stdio.h>
#include <console.h>


typedef struct node{
    int pid;
    struct node * next;
    int ticks;
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

    if(getStateFromPid(runningProc->pid) == RUNNING){
        runningProc->ticks++;
    }
    
    for(int i = 0; i < 3;i++){
        if(getPriorityFromPid(runningProc->pid) == i){
            aux = runningProc;
            toRun = runningProc->next;
           
        } else if(procs[i] != NULL){
            toRun = procs[i]->next;
            aux = procs[i];
        } 
        if(runningProc->ticks == 3){
                lowerPriority(runningProc);
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

    if(getStateFromPid(runningProc->pid) != RUNNING) {
        selectNextProcess(SENTINEL_PID);
    }
}

int getRunningPid(){
    return runningProc->pid;
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
void removeNodefromPriority(node* n ,int p){
    node* aux = procs[p];
    while(aux->next != n)
        aux = aux->next;
    aux->next = n->next;
    return ;
}

void lowerPriority(node* node){
    int nextpriority = getPriorityFromPid(node->pid);
    node->ticks = 0;
    if(nextpriority==LOW)
        return;
    setProcessPriority(node->pid,nextpriority+1);
    return;
}

void changeProcessPriority(int pid, int originalPriority, int newPriority) {
    node* curr = procs[originalPriority];
    node* prev = procsLast[originalPriority];

    if(curr == prev && curr->pid == pid) {
        procs[originalPriority] = NULL;
        procsLast[originalPriority] = NULL;
        return;
    } else {

        while((curr->pid != pid) && (curr != procsLast[originalPriority])) {
            prev = curr;
            curr = curr->next;
        }

        if(curr->pid != pid) {
            //finished the whole list and didn't find anything
            return;
        } else if(curr == procsLast[originalPriority]) {
            procsLast[originalPriority] = prev;
            prev->next = curr->next;
            return;
        } else {
            prev->next = curr->next;
        }
    }
    addNodeToPriority(curr, newPriority);
}

void addToScheduler(int pid) {
    int priority = getPriorityFromPid(pid);
    node * new = allocate(sizeof(node));
    new->pid = pid;
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

