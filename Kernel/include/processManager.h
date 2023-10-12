#include <lib.h>

#ifndef TPE_ARQUI_PROCESSMANAGER_H
#define TPE_ARQUI_PROCESSMANAGER_H

void initializeProcessManager();
uint64_t switchProcess(uint64_t rsp);
void selectNextProcess(int pid);

//if started with ip = NULL, works as fork
int startProcess(uint8_t* ip);

#endif //TPE_ARQUI_PROCESSMANAGER_H
