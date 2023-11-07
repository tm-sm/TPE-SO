#include <interrupts.h>


void processSentinel() {
    while(1) {
        _hlt();
    }
}
