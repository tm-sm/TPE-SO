#include "include/memoryManager.h"
#include <stdio.h>
#include "include/processADT.h"
#define HIGH 0
#define MID 1
#define LOW 2

typedef struct node{
    processADT proc;
    node * next;
}node;


static node * processes[3];
static node * runningProc = NULL;


void roundRobin(){
    node*aux;
    node* toRun;
    for(int i = 0; i < 3;i++){
        if(getPrio(runningProc->proc) == i){
            aux=runningProc;
            toRun=runningProc>next;
        }
        else{
            toRun=processes[i]->next; 
            aux=processes[i];
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


// void round(){
//     node * toRun;
//     int prio = 0;
//     if(getPrio(runningProc->proc) == HIGH){
//         while(prio == 0){
//         toRun = runningProc->next;
//         if(getState(toRun->proc) == READY){
//             loadProcessState(toRun->proc);
//         }
//         toRun = toRun->next;
//         if(toRun == runningProc){
//             prio += 1;
//         }
//         }
//     }
//     for(int i = prio;i < 3; i++){
//         if(getSate(processes[prio]->proc) == READY){
//             runningProc = processes[prio];
//             loadProcessState(processes[prio]);
//         }
//         toRun = processes[prio]->next;
//         while(toRun != processes[prio]){
//             if(getState(toRun->proc) == READY){
//                 runningProc = processes[prio];
//                 loadProcessState(toRun->proc);
//             }
//             toRun = toRun->next;
//         }
//     }
// }

node * initialize(processADT p){
    node new;
    new.proc = p;
    //new.next = highPriority;

    return &new;
}

