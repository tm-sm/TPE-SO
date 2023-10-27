#ifndef TPE_ARQUI_SYSTEM_H
#define TPE_ARQUI_SYSTEM_H

#include <stdint.h>

#define FOREGROUND 1
#define BACKGROUND 0

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

int createProcess(void* ip, int priority, uint8_t foreground, char* name);
void killProcess(int pid);
void setProcessForeground(int pid, int foreground);

#endif //TPE_ARQUI_SYSTEM_H
