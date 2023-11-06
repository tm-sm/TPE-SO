#include <processManager.h>
#include <stdint.h>
#include <scheduler.h>
#include <sems.h>
#include <utils.h>
#include <memoryManager.h>
#define SEM_MAX_AMOUNT 50
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

static sem findSem(char* name){
    for (int i=0;i<SEM_MAX_AMOUNT;i++){
        if(semaphores[i]!=NULL&& strcmp(semaphores[i]->name,name)==0){
               return semaphores[i];
        }
    }
    return NULL;
}
int openSem(char* name, int value){//podria separarse esta funcion para crear y buscar
    sem aux=findSem(name);
    if (aux!=NULL){
        return 2;
    }
    for (int i=0;i<SEM_MAX_AMOUNT;i++){
        if(semaphores[i]==NULL){
            semaphores[i]=allocate(sizeof(semaphore));
            strcpy(semaphores[i]->name,name);
            semaphores[i]->value=value;
            semaphores[i]->lock=0;
            semaphores[i]->processesBlockedAmount=0;
            return 1;
        }
    }
    return 0;
}


int postSem(char* name){
    sem s=findSem(name);
    if(s==NULL){
        return 0;
    }
    enterSem(&(s->lock));
    s->value++;
    if(s->value > 0){
        if(s->processesBlockedAmount > 0){
            int pid = s->processesBlocked[0];
            for(int i=1; i<s->processesBlockedAmount; i++) {
                s->processesBlocked[i-1] = s->processesBlocked[i];
            }
            s->processesBlockedAmount--;
            exitSem(&(s->lock));
            unblockProcess(pid);
        }
    }
    exitSem(&(s->lock));
    return 1;
}


int waitSem(char* name){
    sem s= findSem(name);
    if(s==NULL){
        return 0;
    }
    enterSem(&(s->lock));
    int currentPid=getActiveProcessPid();
   while(s->value == 0){
        s->processesBlocked[s->processesBlockedAmount] = currentPid;
        s->processesBlockedAmount++;
        exitSem(&(s->lock));
        blockCurrentProcess();
        enterSem(&(s->lock));
    }
    s->value--;
    exitSem(&(s->lock));
    return 1;
}
int closeSem(char* name){
    sem s=findSem(name);
    if(s==NULL){
        return 0;
    }
    deallocate(s);
        return 1;
}

int getSemValue(char* name) {
    sem s=findSem(name);
    if(s==NULL) {
        return 0;
    }
    return s->value;
}


