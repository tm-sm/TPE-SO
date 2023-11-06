#ifndef SEMS_H
#define SEMS_H

typedef struct semaphore* sem;
int openSem(char* name,int value);
int postSem(char* name);
int waitSem(char* name);
int closeSem(char* name);
int getSemValue(char* name);



#endif

