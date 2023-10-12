#include "lib.h"
#include "memoryManager.h"
#include "processManager.h"

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3
#define INIT_STACK_SIZE 1024
#define MAXPROCESSES 10
#define MAX_PROC 10

void roundRobin();
void addToSchedule(proc p);

#endif //SCHEDULER
