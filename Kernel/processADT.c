#include <stdint.h>
#include <stdio.h>
#include <processADT.h>
#include <string.h>
#include <memoryManager.h>
#define STACK_SIZE 2048
#define STATE_SIZE 17 //all r registers
#define MAXPROCESSES 10

processADT ForegroundProcess; //esto podria ir en scheduler.c

struct processCDT{ //process control block
    char pname[20];
    int pid;
    int state;
    uint64_t regs[STATE_SIZE];
} processCDT;

processADT PT[MAXPROCESSES]; //process table

processADT birthProcess(char*name) {
    int i=getSpace();
    if (i==-1)
        return NULL;
    memcpy(PT[i]->pname,name,20);
    PT[i]->pid=i;
    PT[i]->state=READY;
}

int loadProcessState(processADT p) {

}

int saveProcessState(processADT p) {

}

int getSpace(){
    for(int i=0;i<MAXPROCESSES;i++)
        if(PT[i]->state==DEAD)
            return i;
    return -1;
}

processADT getProcess(int pid){
for(int i=0;i<MAXPROCESSES;i++)
    if(PT[i]->pid==pid)
        return PT[i];
    return NULL;
}
void BeheadProcess(int pid){
    processADT p=getProcess(pid);
    p->state=DEAD;
}


