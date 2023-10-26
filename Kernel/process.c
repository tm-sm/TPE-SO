#include <console.h>


void processSentinel() {
    cPrint("Sentinel process started");
    while(1);
}

void processHello() {
    while(1) {
        cPrint("\nHello");
    }
}

void processWorld() {
    while(1) {
        cPrint("          World\n");
    }
}