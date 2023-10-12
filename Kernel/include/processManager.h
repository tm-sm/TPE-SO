#include "lib.h"

#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3
#define INIT_STACK_SIZE 1024
#define MAXPROCESSES 10
#define MAX_PROC 10

typedef struct process* proc;
void initializeProcessManager();
uint64_t switchProcess(uint64_t rsp);
void selectNextProcess(int pid);

//if started with ip = NULL, works as fork
int startProcess(uint8_t* ip, int priority);
int getPriority(proc p);

#endif //TPE_ARQUI_PROCESSMANAGER_H
