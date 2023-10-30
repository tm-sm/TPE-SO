#include <processManager.h>
#include <stdint.h>
#include <scheduler.h>
#include <sems.h>
#include <utils.h>
#include <memoryManager.h>
#define SEM_MAX_AMOUNT 10
typedef struct semaphore{
    int value;
    char name[24];
    int lock;
    int processesBlocked[10];
    int processesBlockedAmount;
}semaphore;

sem semaphores[SEM_MAX_AMOUNT]={NULL};

//ver de implementar las operaciones atomicas en asm
// con post y wait
sem openSem(char* name,int value){
    for (int i=0;i<SEM_MAX_AMOUNT;i++){
        if(strcmp(semaphores[i]->name,name)==0)
            return semaphores[i];
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

void closeSem(char* name){
    for(int i=0;i<SEM_MAX_AMOUNT;i++){
        if(strcmp(semaphores[i]->name,name)==0){
            deallocate(semaphores[i]);
            return ;
        }
    }
}


