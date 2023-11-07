#include "lib.h"
#include "memoryManager.h"
#include "processManager.h"

#ifndef SCHEDULER_H
#define SCHEDULER_H

#define HIGH 0
#define MED 1
#define LOW 2
#define UNDEFINED 3

// Initialize the scheduler
void initializeScheduler();

// Execute the scheduler to select the next process to run
void scheduler();

/**
 * Add a process to the scheduler with a given process ID (pid).
 *
 * @param pid Process ID of the process to add to the scheduler
 */
void addToScheduler(int pid);

/**
 * Remove a process from the scheduler with a given process ID (pid) and priority.
 *
 * @param pid             Process ID of the process to remove from the scheduler
 * @param priority        Priority of the process to remove from the scheduler
 */
void removeFromScheduler(int pid, int priority);

/**
 * Change the priority of a process with a given process ID (pid) from the original priority to the new priority.
 *
 * @param pid               Process ID of the process to change the priority for
 * @param originalPriority  Original priority of the process
 * @param newPriority       New priority for the process
 */
void changeProcessPriority(int pid, int originalPriority, int newPriority);

/**
 * Print the list of processes with a specific priority.
 *
 * @param priority Priority level to list processes for
 */
void printPriorityList(int priority);

// Get the process ID (pid) of the currently running process
int getRunningPid();

#endif //SCHEDULER
