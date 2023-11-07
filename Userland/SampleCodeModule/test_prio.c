#include <stdint.h>
#include <stdio.h>
#include "system.h"
#include "standardLib.h"
#include "test_util.h"

#define MINOR_WAIT 1000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 3000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 2 // TODO: Change as required
#define MEDIUM 1  // TODO: Change as required
#define HIGHEST 0 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = createProcess(endless_loop,2,BACKGROUND,0,"endless_loop",argv);

  bussy_wait(WAIT);
  printFormat("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    setProcessPriority(pids[i], prio[i]);

  bussy_wait(WAIT);
  printFormat("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    blockProcess(pids[i]);

  printFormat("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    setProcessPriority(pids[i], MEDIUM);

  printFormat("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    unblockProcess(pids[i]);

  bussy_wait(WAIT);
  printFormat("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
   killProcess(pids[i]);
}
