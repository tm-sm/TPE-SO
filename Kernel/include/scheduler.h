#include "lib.h"
#include "memoryManager.h"
#include "processManager.h"

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define HIGH 0
#define MED 1
#define LOW 2

void scheduler();
void addToScheduler(int pid);

#endif //SCHEDULER
