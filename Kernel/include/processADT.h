#ifndef TPE_ARQUI_PROCESSADT_H
#define TPE_ARQUI_PROCESSADT_H

typedef struct processCDT * processADT;
#define READY 0
#define BLOCKED -1
#define RUNNING 1
#define DEAD -8


processADT birthProcess(char*name);
int loadProcessState(processADT p);
int saveProcessState(processADT p);
int getSpace();
processADT getProcess(int pid);
void BeheadProcess(int pid);

#endif //TPE_ARQUI_PROCESSADT_H
