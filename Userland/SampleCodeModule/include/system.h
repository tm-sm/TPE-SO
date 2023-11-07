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
 * Checks whether a key is currently being pressed or not.
 *
 * @return 1 if a key press was detected, 0 if not
 */
uint8_t keyPress();

/**
 * Waits for a specified number of milliseconds.
 *
 * @param milliseconds The number of milliseconds to wait
 */
void wait(uint64_t milliseconds);

/**
 * Plays a beep sound with the specified frequency for a certain amount of time.
 *
 * @param frequency     Frequency for the beep
 * @param milliseconds  Amount of time the beep should play for
 */
void play_beep(uint32_t frequency, uint32_t milliseconds);

/**
 * Create a new process.
 *
 * @param ip         Entry point function for the process
 * @param priority   Priority of the process
 * @param foreground Indicates if the process should run in the foreground
 * @param isBlocked  Flag to specify if the process is initially blocked
 * @param name       Name of the process
 * @param argv       Array of command-line arguments
 * @return           The process ID (pid) of the newly created process
 */
int createProcess(void* ip, int priority, int foreground, int isBlocked, char* name, char* argv[]);

/**
 * Allocate memory of a specified size.
 *
 * @param size Size of memory to allocate
 * @return     Pointer to the allocated memory
 */
void* alloc(size_t size);

/**
 * Reallocate memory at a given address with a new size.
 *
 * @param address Address of the memory to reallocate
 * @param size    New size for the memory
 * @return        Pointer to the reallocated memory
 */
void* realloc(void* address, size_t size);

/**
 * Deallocate memory at a given address.
 *
 * @param address Address of the memory to deallocate
 */
void dealloc(void* address);

/**
 * Get the amount of available memory.
 *
 * @return The amount of available memory in bytes
 */
int getAvailableMemory();

/**
 * Kill a process with a given process ID (pid).
 *
 * @param pid Process ID (pid) of the process to kill
 */
void killProcess(int pid);

/**
 * Set the foreground status of the current process.
 *
 * @param foreground Flag to specify if the current process should run in the foreground
 */
void setOwnForeground(int foreground);

/**
 * Set the foreground status of a process with a given process ID (pid).
 *
 * @param pid        Process ID (pid) of the process to set the foreground status for
 * @param foreground Flag to specify if the process should run in the foreground
 */
void setProcessForeground(int pid, int foreground);

/**
 * Check if a process with a given process ID (pid) is running in the foreground.
 *
 * @param pid Process ID (pid) to check
 * @return    1 if the process is in the foreground, 0 if not
 */
int isProcessInForeground(int pid);

/**
 * Check if a process with a given process ID (pid) is alive.
 *
 * @param pid Process ID (pid) to check
 * @return    1 if the process is alive, 0 if not
 */
int isProcessAlive(int pid);

/**
 * Print information about all processes.
 */
void printAllProcesses();

/**
 * Get the process ID (pid) of the current process.
 *
 * @return The process ID (pid) of the current process
 */
int getOwnPid();

/**
 * Exit the current process.
 */
void exitProc();

/**
 * Set the priority of a process with a given process ID (pid).
 *
 * @param pid      Process ID (pid) of the process to set the priority for
 * @param priority Priority value to set for the process
 */
void setProcessPriority(int pid, int priority);

/**
 * Check if a process with a given process ID (pid) is blocked.
 *
 * @param pid Process ID (pid) to check
 * @return    1 if the process is blocked, 0 if not
 */
int isProcessBlocked(int pid);

/**
 * Block a process with a given process ID (pid).
 *
 * @param pid Process ID (pid) to block
 */
void blockProcess(int pid);

/**
 * Unblock a process with a given process ID (pid).
 *
 * @param pid Process ID (pid) to unblock
 */
void unblockProcess(int pid);

/**
 * Wait for child processes to exit.
 */
void waitForChildren();

/**
 * Wait for a specific child process to exit using its process ID (pid).
 *
 * @param pid Process ID (pid) of the child process to wait for
 */
void waitForChild(int pid);
void yieldProcess();

/**
 * Open or create a semaphore with a given name and initial value.
 *
 * @param name  Name of the semaphore
 * @param value Initial value for the semaphore
 * @return      An integer representing the success or failure of the operation
 */
int openSem(char* name, int value);

/**
 * Post (signal) a semaphore with a given name.
 *
 * @param name Name of the semaphore to post
 * @return     An integer representing the success or failure of the operation
 */
int postSem(char* name);

/**
 * Wait (block) on a semaphore with a given name.
 *
 * @param sem Name of the semaphore to wait on
 * @return    An integer representing the success or failure of the operation
 */
int waitSem(char* sem);

/**
 * Destroy a semaphore with a given name.
 *
 * @param sem Name of the semaphore to destroy
 * @return    An integer representing the success or failure of the operation
 */
int destroySem(char* sem);

/**
 * Create a pipe for inter-process communication, returning file descriptors.
 *
 * @param fd Array to hold the two file descriptors for the pipe
 * @return   An integer representing the success or failure of the operation
 */
int createPipe(int fd[2]);

/**
 * Close a pipe, releasing associated resources.
 *
 * @param fd File descriptor for the pipe to close
 */
void closePipe(int fd);

/**
 * Connect two processes for communication.
 *
 * @param p1 Process ID of the first process to connect
 * @param p2 Process ID of the second process to connect
 * @return   An integer representing the success or failure of the operation
 */
int connectProcesses(int p1, int p2);

/**
 * Display information about FIFOs (Named Pipes).
 *
 * @return An integer representing the success or failure of the operation
 */
int displayFIFOs();

/**
 * Connect two processes using a named pipe (FIFO) and their process IDs (pids).
 *
 * @param name Name of the named pipe
 * @param p1   Process ID of the first process to connect
 * @param p2   Process ID of the second process to connect
 * @return     An integer representing the success or failure of the operation
 */
int connectToFIFO(const char * name, int p1, int p2);

#endif //TPE_ARQUI_SYSTEM_H
