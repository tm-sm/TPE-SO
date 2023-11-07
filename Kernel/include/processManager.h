#include "lib.h"

#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3
#define INIT_STACK_SIZE 2048
#define MAX_PROC 17 // 16 processes for the user + sentinel

#define PROC_NAME_LENGTH 20

#define FOREGROUND 1
#define BACKGROUND 0

#define SENTINEL_PID 0
#define SENTINEL_STACK_SIZE 256
typedef struct Process* proc;

// Initialize the process manager
void initializeProcessManager();

// Switch to the next process and save its stack pointer
uint64_t switchProcess(uint64_t rsp);

// Select the next process to run based on its process ID (pid)
void selectNextProcess(int pid);

/**
 * Start a new process.
 *
 * @param ip        Entry point function for the process (NULL for fork)
 * @param priority  Priority of the process
 * @param foreground Indicates if the process should run in the foreground
 * @param isBlocked Flag to specify if the process is initially blocked
 * @param name      Name of the process
 * @param stackSize Stack size for the process
 * @param argv      Array of command-line arguments
 * @return          The process ID (pid) of the newly started process
 */
int startProcess(void* ip, int priority, int foreground, int isBlocked, const char* name, unsigned int stackSize, char* argv[]);

/**
 * Get the process ID (pid) associated with a Process structure pointer.
 *
 * @param p Process structure pointer
 * @return  The process ID (pid) associated with the process
 */
int getPid(proc p);

/**
 * Get the priority associated with a process.
 *
 * @param p Process structure pointer
 * @return  The priority of the process
 */
int getPriority(proc p);

/**
 * Get the priority associated with a process using its process ID (pid).
 *
 * @param pid Process ID (pid)
 * @return    The priority of the process
 */
int getPriorityFromPid(int pid);

/**
 * Get the state of a process using its process ID (pid).
 *
 * @param pid Process ID (pid)
 * @return    The state of the process
 */
int getStateFromPid(int pid);

// Get the process ID (pid) of the currently active process
int getActiveProcessPid();

// Get the process ID (pid) of the process running in the foreground
int getForegroundPid();

/**
 * Get the file descriptor for the standard input (stdin) of a process using its process ID (pid).
 *
 * @param pid Process ID (pid)
 * @return    The file descriptor for the stdin of the process
 */
int getStdinFd(int pid);

/**
 * Get the file descriptor for the standard output (stdout) of a process using its process ID (pid).
 *
 * @param pid Process ID (pid)
 * @return    The file descriptor for the stdout of the process
 */
int getStdoutFd(int pid);

// Set a process to run in the foreground or background
void setProcessForeground(int pid, int foreground);

// Set the priority of a process using its process ID (pid)
void setProcessPriority(int pid, int priority);

// Check if a process with a given process ID (pid) is alive
int isProcessAlive(int pid);

// Check if a process with a given process ID (pid) is running in the foreground
int isProcessInForeground(int pid);

// Check if the current process is running in the foreground
int isCurrentProcessInForeground();

// List information about all running processes
void listAllProcesses();

// Block the currently running process
void blockCurrentProcess();

// Block a specific process using its process ID (pid)
void blockProcess(int pid);

// Unblock a specific process using its process ID (pid)
void unblockProcess(int pid);

void yieldProcess();

// Connect two processes by their process IDs (pids)
int connectProcs(int pidProc1, int pidProc2);

/**
 * Connect two processes using a named pipe and their process IDs (pids).
 *
 * @param name      Name of the named pipe
 * @param pidProc1  Process ID of the first process
 * @param pidProc2  Process ID of the second process
 * @return          An integer representing the success or failure of the operation
 */
int connectToNamedPipe(const char* name, int pidProc1, int pidProc2);

// Wait for a child process to exit using its process ID (pid)
void waitForChild(int pid);

// Wait for all child processes to exit
void waitForChildren();

// Kill a specific process using its process ID (pid)
void killProcess(int pid);

// Kill the process running in the foreground
void killProcessInForeground();

// Exit the current process
void exitProc();

#endif //TPE_ARQUI_PROCESSMANAGER_H
