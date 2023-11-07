#include <stdint.h>
#include <system.h>
#include <syscall.h>
#include <standardLib.h>
/**
| * Syscalls wrappers for the test files
|*/

int64_t my_getpid() {
  getOwnPid();
}

int64_t my_create_process(void* ip,char *name, uint64_t argc, char *argv[]) {
  return createProcess(ip, HIGH,BACKGROUND, 0, name, argv);
}

int64_t my_nice(uint64_t pid, uint64_t newPrio) {
  setProcessPriority(pid, newPrio);
  return 0;
}

int64_t my_kill(uint64_t pid) {
  killProcess(pid);
}

int64_t my_block(uint64_t pid) {
  blockProcess(pid);
}

int64_t my_unblock(uint64_t pid) {
  unblockProcess(pid);
  return 0;
}

int64_t my_sem_open(char *sem_id, uint64_t initialValue) {
  return openSem(sem_id, initialValue);
}

int64_t my_sem_wait(char *sem_id) {
  return waitSem(sem_id);
}

int64_t my_sem_post(char *sem_id) {
  return postSem(sem_id);
}

int64_t my_sem_close(char *sem_id) {
  return destroySem(sem_id);
}

int64_t my_yield() {
  yield();
  return 0;
}

int64_t my_wait(int64_t pid) {
  waitForChildren(pid);
  return 0;
}
