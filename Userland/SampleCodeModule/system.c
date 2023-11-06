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
#define SYS_CHECK_PROCESS_FOREGROUND 16
#define SYS_PRINT_ALL_PROCESSES 17
#define SYS_PROCESS_PRIORITY 18
#define SYS_PROCESS_BLOCK 19
#define SYS_WAIT_FOR_CHILDREN 20
#define SYS_OPEN_SEM 21
#define SYS_POST_SEM 22
#define SYS_WAIT_SEM 23
#define SYS_CLOSE_SEM 24
#define SYS_CREATE_PIPE 25
#define SYS_CLOSE_PIPE 26
#define SYS_CONNECT_PROCESSES 27
#define SYS_DISPLAY_FIFO 28
#define SYS_CONNECT_TO_FIFO 29

#define ALLOC 0
#define REALLOC 1
#define DEALLOC 2
#define GET_AVAILABLE_MEMORY 3

#define BLOCK_GET 0
#define BLOCK_SET 1
#define BLOCK_BLOCK 1
#define BLOCK_UNBLOCK 0
#define PRIORITY_GET 0
#define PRIORITY_SET 1

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

int createProcess(void* ip, int priority, int foreground, int isBlocked, char* name, char* argv[]) {
    uint64_t aux = join(priority, foreground);
    return (int)interrupt(SYS_CREATE_PROCESS, (uint64_t)ip, aux, isBlocked, (uint64_t)name, (uint64_t)argv);
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

int isProcessInForeground(int pid) {
    return (int)interrupt(SYS_CHECK_PROCESS_FOREGROUND, pid, 0, 0, 0, 0);
}

int getOwnPid() {
    return (int)interrupt(SYS_GET_OWN_PID, 0, 0, 0, 0, 0);
}

void exitProc() {
    int pid = getOwnPid();
    killProcess(pid);
}

int isProcessAlive(int pid) {
    return (int)interrupt(SYS_IS_PROCESS_ALIVE, pid, 0, 0, 0, 0);
}

void* alloc(size_t size) {
    return (void*)interrupt(SYS_MEMORY_MANAGER, ALLOC, 0, size, 0, 0);
}

void* realloc(void* address, size_t size) {
    return (void*)interrupt(SYS_MEMORY_MANAGER, REALLOC, (uint64_t)address, size, 0, 0);
}

void dealloc(void* address) {
    interrupt(SYS_MEMORY_MANAGER, DEALLOC, (uint64_t)address, 0, 0, 0);
}

int getAvailableMemory() {
    return (int)interrupt(SYS_MEMORY_MANAGER, GET_AVAILABLE_MEMORY, 0, 0, 0, 0);
}

void printAllProcesses() {
    interrupt(SYS_PRINT_ALL_PROCESSES, 0, 0 ,0 ,0, 0);
}

void setProcessPriority(int pid, int priority) {
    interrupt(SYS_PROCESS_PRIORITY, pid, PRIORITY_SET, priority, 0, 0);
}

int isProcessBlocked(int pid) {
    return (int)interrupt(SYS_PROCESS_BLOCK, pid, BLOCK_GET, 0, 0, 0);
}

void blockProcess(int pid) {
    interrupt(SYS_PROCESS_BLOCK, pid, BLOCK_SET, BLOCK_BLOCK, 0, 0);
}

void unblockProcess(int pid) {
    interrupt(SYS_PROCESS_BLOCK, pid, BLOCK_SET, BLOCK_UNBLOCK, 0, 0);
}

void waitForChildren() {
    interrupt(SYS_WAIT_FOR_CHILDREN, 0, 0, 0, 0, 0);
}

void waitForChild(int pid) {
    if(pid <= 2) {
        return;
    }
    interrupt(SYS_WAIT_FOR_CHILDREN, pid, 0, 0, 0, 0);
}

int openSem(char* name,int value){
    return (int)interrupt(SYS_OPEN_SEM,(uint64_t)name,value,0,0,0);
}

int postSem(char* name){
    return (int)interrupt(SYS_POST_SEM,(uint64_t)name,0,0,0,0);
}

int waitSem(char* name){
    return (int)interrupt(SYS_WAIT_SEM,(uint64_t)name,0,0,0,0);
}

int destroySem(char* name){
    return (int)interrupt(SYS_CLOSE_SEM,(uint64_t)name,0,0,0,0);
}

int createPipe(int fd[2]) {
    return (int)interrupt(SYS_CREATE_PIPE, (uint64_t)fd, 0, 0, 0, 0);
}

void closePipe(int fd) {
    interrupt(SYS_CLOSE_PIPE, (uint64_t)fd, 0, 0, 0, 0);
}

int connectToFIFO(const char * name, int p1, int p2){
    return (int)interrupt(SYS_CONNECT_TO_FIFO, (uint64_t)name,(uint64_t)p1, (uint64_t)p2, 0, 0);
}

int displayFIFOs(){
    return (int)interrupt(SYS_DISPLAY_FIFO, 0,0, 0, 0, 0);
}

int connectProcesses(int p1, int p2) {
    return (int)interrupt(SYS_CONNECT_PROCESSES, (uint64_t)p1, (uint64_t)p2, 0, 0, 0);
}

