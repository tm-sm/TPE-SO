#include <console.h>
#include <processManager.h>
#include <interrupts.h>


void processSentinel() {
    while(1) {
        _hlt();
    }
}

void processHello() {
    char* alo = allocate(sizeof(char) * 25);
    read(0,alo,25);
    write(1,alo,25);
    exitProc();
}

void processHello2(int argc, char* argv[]) {
    while(1) {
        for(int i=0; i<argc; i++) {
            cPrint(argv[i]);
        }
    }
}

void processWorld() {
    while(1) {
        cPrint("          World\n");
    }
}