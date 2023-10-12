#include <time.h>
#include <stdint.h>
#include <keyboardDriver.h>
#include <processManager.h>

static void int_20(uint64_t* registers), int_21(uint64_t* registers);

static void (*handlerArr[])(uint64_t* registers) = {int_20, int_21};

void irqDispatcher(uint64_t irq, uint64_t* registers) {
    handlerArr[irq](registers);
}

void int_20(uint64_t* registers) {
    timer_handler();
    //TODO agregar llamado al scheduler
    selectNextProcess(0); //carga el shell
}

void int_21(uint64_t* registers) {
    keyboard_handler(registers);
}
