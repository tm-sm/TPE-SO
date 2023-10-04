#include <stdint.h>
#include <processADT.h>

#define STACK_SIZE 2048
#define STATE_SIZE 17 //all r registers

struct processCDT{
    int pid;
    uint8_t stack_ptr[STACK_SIZE];
    uint64_t state[STATE_SIZE];
} processCDT;

int startProcess(processADT p) {

}

int loadProcessState(processADT p) {

}

int saveProcessState(processADT p) {

}