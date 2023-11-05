#include "lib.h"

#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3
#define INIT_STACK_SIZE 2048
#define MAX_PROC 11 // 10 processes for the user + sentinel

#define PROC_NAME_LENGTH 20

#define FOREGROUND 1
#define BACKGROUND 0

#define SENTINEL_PID 0
#define SENTINEL_STACK_SIZE 256

typedef struct process* proc;
void initializeProcessManager();
uint64_t switchProcess(uint64_t rsp);
void selectNextProcess(int pid);

//if started with ip = NULL, works as fork
int startProcess(void* ip, int priority, uint8_t foreground, const char* name, unsigned int stackSize, char* argv[]);
int getPid(proc p);
int getPriorityFromPid(int pid);
int getPriority(proc p);
int getStateFromPid(int pid);
void killProcess(int pid);
void setProcessForeground(int pid, int foreground);
int isProcessAlive(int pid);
int getActiveProcessPid();
void listAllProcesses();
void removeFromFgStack(int pid);
void addToFgStack(int pid);
void blockCurrentProcess();
void blockProcess(int pid);
void unblockProcess(int pid);
void setProcessPriority(int pid, int priority);
void killProcessInForeground();

int getStdinFd(int pid);
int getStdoutFd(int pid);
int connectProcs(int pidProc1, int pidProc2);

void waitForChild(int pid);
void waitForChildren();
void notifyParent(int parentPid, int childPid);
void exitProc();

int isProcessInForeground(int pid);
int isCurrentProcessInForeground();

#endif //TPE_ARQUI_PROCESSMANAGER_H
