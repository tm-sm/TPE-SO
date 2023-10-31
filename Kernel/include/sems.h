#ifndef SEMS_H
#define SEMS_H

typedef struct semaphore* sem;
sem openSem(char* name,int value);
void postSem(sem s);
void waitSem(sem s);
void closeSem(char* name);



#endif

