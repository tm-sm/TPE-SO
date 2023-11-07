#include <stdint.h>
#include <stdio.h>
#include <system.h>
#include "standardLib.h"
#include "test_util.h"

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  yield();  // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if (use_sem)
    if (!openSem(SEM_ID, 1)) {
      printFormat("test_sync: ERROR opening semaphore\n");
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      waitSem(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      postSem(SEM_ID);
  }

  if (use_sem)
    destroySem(SEM_ID);

  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
    return -1;

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;

  //wtf is process_inc
  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = createProcess(my_process_inc, 0,BACKGROUND,0,"process_inc", argvDec);
    pids[i + TOTAL_PAIR_PROCESSES] = createProcess(my_process_inc, 2,BACKGROUND,0,"process_inc", argvInc);
  }
  //No clue
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    waitForChild(pids[i]);
    waitForChild(pids[i + TOTAL_PAIR_PROCESSES]);
  }

  printFormat("Final value: %d\n", global);

  return 0;
}
