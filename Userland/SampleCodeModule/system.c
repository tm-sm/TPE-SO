#include <stdint.h>
#include <standardLib.h>

#define SYS_DETECT_KEY_PRESS_ID 5
#define SYS_WAIT_ID 6
#define SYS_SOUND_ID 7
#define SYS_CREATE_PROCESS 10
#define SYS_KILL_PROCESS 11
#define SYS_SET_PROCESS_FOREGROUND 12

extern uint64_t* current_regs();

uint8_t keyPress() {
    uint8_t b;
    interrupt(SYS_DETECT_KEY_PRESS_ID, (uint64_t)&b, 0, 0, 0, 0);
    return b;
}

void wait(uint64_t milliseconds) {
    interrupt(SYS_WAIT_ID, milliseconds, 0, 0, 0, 0);
}

void play_beep(uint32_t frequency, uint32_t duration) {
    interrupt(SYS_SOUND_ID, frequency, duration,  0, 0, 0);
}

int createProcess(void* ip, int priority, uint8_t foreground, char* name) {
    return (int)interrupt(SYS_CREATE_PROCESS, (uint64_t)ip, priority, foreground, (uint64_t)name, 0);
}

void killProcess(int pid) {
    interrupt(SYS_KILL_PROCESS, pid, 0, 0, 0, 0);
}

void setProcessForeground(int pid, int foreground) {
    interrupt(SYS_SET_PROCESS_FOREGROUND, pid, foreground, 0, 0, 0);
}
