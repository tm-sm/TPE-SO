#include <lib.h>

#ifndef TPE_ARQUI_PROCESSMANAGER_H
#define TPE_ARQUI_PROCESSMANAGER_H

void initializeProcessManager();
void switchProcess(uint64_t* registers, int pid);
void startProcess();

#endif //TPE_ARQUI_PROCESSMANAGER_H
