#ifndef TPE_ARQUI_SYSTEM_H
#define TPE_ARQUI_SYSTEM_H

#include <stdint.h>
#include <stddef.h>

#define FOREGROUND 1
#define BACKGROUND 0

#define HIGH 0
#define MED 1
#define LOW 2

/**
 * Checks whether a key is currently being pressed or not
 * @return 1 if a key press was detected, 0 if not
 */
uint8_t keyPress();

/**
 * Waits for a specified number of milliseconds
 * @param milliseconds
 */
void wait(uint64_t milliseconds);

/**
 * Plays a beep sound
 * @param frequency Frequency for the beep
 * @param milliseconds Amount of time the beep should play for
 */
void play_beep(uint32_t frequency, uint32_t milliseconds);

int createProcess(void* ip, int priority, uint8_t foreground, char* name, char* argv[]);
void* alloc(size_t size);
void* realloc(void* address, size_t size);
void dealloc(void* address);
int getAvailableMemory();
void killProcess(int pid);
void setOwnForeground(int foreground);
void setProcessForeground(int pid, int foreground);
int isProcessInForeground(int pid);
int isProcessAlive(int pid);
void printAllProcesses();
int getOwnPid();
void exitProc();

void setProcessPriority(int pid, int priority);
int isProcessBlocked(int pid);
void blockProcess(int pid);
void unblockProcess(int pid);
void waitForChildren();
void waitForChild(int pid);

int openSem(char* name, int value);
int postSem(char* name);
int waitSem(char* sem);
int destroySem(char* sem);

#endif //TPE_ARQUI_SYSTEM_H
