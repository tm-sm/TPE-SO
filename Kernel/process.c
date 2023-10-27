#include <console.h>
#include <processManager.h>


void processSentinel() {
    cPrint("Sentinel process started");
    while(1);
}

void processHello() {
    for(int i=0; i<10; i++) {
        cPrint("\nHello");
    }
    killProcess(getActiveProcessPid());
}

void processHello2() {
    for(int i=0; i<10; i++) {
        cPrint("\nHello2");
    }
    killProcess(getActiveProcessPid());
}

void processWorld() {
    while(1) {
        cPrint("          World\n");
    }
}