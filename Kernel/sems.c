#include <processManager.h>
#include <stdint.h>
#include <scheduler.h>
#include <sems.h>
#include <utils.h>
#include <memoryManager.h>
#define SEM_MAX_AMOUNT 10
#define MAX_PROCESSES_BLOCKED 10

typedef struct semaphore{
    int value;
    char name[24];
    int lock;
    int processesBlocked[MAX_PROCESSES_BLOCKED]; //TODO maybe cambiar por lista
    int processesBlockedAmount;
}semaphore;

sem semaphores[SEM_MAX_AMOUNT]={NULL};

void enterSem(int* lock);
void exitSem(int* lock);

sem openSem(char* name, int value){//podria separarse esta funcion para crear y buscar
    for (int i=0;i<SEM_MAX_AMOUNT;i++){
        if(strcmp(semaphores[i]->name,name)==0)
            return semaphores[i];
    }
    for (int i=0;i<SEM_MAX_AMOUNT;i++){
        if(semaphores[i]==NULL){
            semaphores[i]=allocate(sizeof(semaphore));
            strcpy(semaphores[i]->name,name);
            semaphores[i]->value=value;
            semaphores[i]->lock=0;
            semaphores[i]->processesBlockedAmount=0;
            return semaphores[i];
        }
    }
    return NULL;
}

void postSem(sem s){
    enterSem(&(s->lock));
    s->value++;
    if(s->value>0){
        for(int i=0;i<s->processesBlockedAmount;i++){
            unblockProcess(s->processesBlocked[i]);
        }
        s->processesBlockedAmount=0;
    }
    exitSem(&(s->lock));
}


void waitSem(sem s){
    enterSem(&(s->lock));
    int currentpid=getActiveProcessPid();
   if(s->value==0){
        s->processesBlocked[s->processesBlockedAmount]=currentpid;
        s->processesBlockedAmount++;
        exitSem(&(s->lock));
        blockCurrentProcess();
    }
    else{
        s->value--;
        exitSem(&(s->lock));
    }
}
void closeSem(char* name){
    for(int i=0;i<SEM_MAX_AMOUNT;i++){
        if(strcmp(semaphores[i]->name,name)==0){
            if(semaphores[i]->processesBlockedAmount!=0)
                deallocate(semaphores[i]);
            return ;
        }
    }
}


