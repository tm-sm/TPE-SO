#include "include/scheduler.h"
#include <stdio.h>

#define HIGH 0
#define MID 1
#define LOW 2

typedef struct node{
    proc proc;
    node * next;
}node;


static node * procs[3];
static node * runningProc = NULL;


void roundRobin(){
    node*aux;
    node* toRun;
    for(int i = 0; i < 3;i++){
        if(getPrio(runningProc->proc) == i){
            aux=runningProc;
            toRun=runningProc->next;
        }
        else{
            toRun=procs[i]->next; 
            aux=procs[i];
            }
        while(aux != toRun){
                if(getState(toRun->proc) == READY){
                    runningProc = toRun;                    
                    loadProcessState(runningProc->proc);
                }
                toRun=toRun->next;
            }
        
    }
    //halt
}

void addToSchedule(proc p){
    node * check = procs[getPriority(p)]->next;
    while(check->next != procs[getPriority(p)]){
        check = check->next;
    }
    check->next = initialize(p);
}


node * initialize(proc p){
    node new;
    new.proc = p;
    new.next = procs[getPriority(p)];
    return &new;
}

