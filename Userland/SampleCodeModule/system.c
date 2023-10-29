#include <stdint.h>
#include <standardLib.h>
#include <system.h>
#include <stddef.h>

#define SYS_DETECT_KEY_PRESS_ID 5
#define SYS_WAIT_ID 6
#define SYS_SOUND_ID 7
#define SYS_CREATE_PROCESS 10
#define SYS_KILL_PROCESS 11
#define SYS_SET_PROCESS_FOREGROUND 12
#define SYS_GET_OWN_PID 13
#define SYS_IS_PROCESS_ALIVE 14
#define SYS_MEMORY_MANAGER 15

#define ALLOC_ID 0
#define REALLOC_ID 1
#define DEALLOC_ID 2

extern uint64_t* current_regs();

uint8_t keyPress() {
    return interrupt(SYS_DETECT_KEY_PRESS_ID, 0, 0, 0, 0, 0);
}

void wait(uint64_t milliseconds) {
    interrupt(SYS_WAIT_ID, milliseconds, 0, 0, 0, 0);
}

void play_beep(uint32_t frequency, uint32_t duration) {
    interrupt(SYS_SOUND_ID, frequency, duration,  0, 0, 0);
}

int createProcess(void* ip, int priority, uint8_t foreground, char* name, char* argv[]) {
    return (int)interrupt(SYS_CREATE_PROCESS, (uint64_t)ip, priority, foreground, (uint64_t)name, (uint64_t)argv);
}

void killProcess(int pid) {
    interrupt(SYS_KILL_PROCESS, pid, 0, 0, 0, 0);
}

void setOwnForeground(int foreground) {
    int pid = (int)interrupt(SYS_GET_OWN_PID, 0, 0, 0, 0, 0);
    setProcessForeground(pid, foreground);
}

void setProcessForeground(int pid, int foreground) {
    interrupt(SYS_SET_PROCESS_FOREGROUND, pid, foreground, 0, 0, 0);
}

void exitProc(int argc, char* argv[]) {
    int pid = (int)interrupt(SYS_GET_OWN_PID, 0, 0, 0, 0, 0);
    argv--;
    for(int i=0; i<argc + 1; i++) {
        dealloc(argv[i]);
    }
    dealloc(argv);
    killProcess(pid);
}

int isProcessAlive(int pid) {
    return (int)interrupt(SYS_IS_PROCESS_ALIVE, pid, 0, 0, 0, 0);
}

void* alloc(size_t size) {
    return (void*)interrupt(SYS_MEMORY_MANAGER, ALLOC_ID, 0, size, 0, 0);
}

void* realloc(void* address, size_t size) {
    return (void*)interrupt(SYS_MEMORY_MANAGER, REALLOC_ID, (uint64_t)address, size, 0, 0);
}

void dealloc(void* address) {
    interrupt(SYS_MEMORY_MANAGER, DEALLOC_ID, (uint64_t)address, 0, 0, 0);
}
