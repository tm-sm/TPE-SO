#include "lib.h"
#include "memoryManager.h"
#include "processManager.h"

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define HIGH 0
#define MED 1
#define LOW 2
#define UNDEFINED 3

void scheduler();
void addToScheduler(int pid);
void removeFromScheduler(int pid, int priority);
void printPriorityList(int priority);
int getRunningPid();
#endif //SCHEDULER
