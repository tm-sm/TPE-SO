#include <time.h>
#include <stdint.h>
#include <processManager.h>
#include <memoryManager.h>
#include <scheduler.h>

#define TRUE 1
#define FALSE 0

#define SECONDS 0X00
#define MINUTES 0X02
#define HOURS 0x04
#define DAYS 0X07
#define MONTH 0X08
#define YEAR 0X09
#define TIME_ZONE -3
#define TICK_MS 55

struct waitNode {
    int remainingTicks;
    int pid;
    struct waitNode* next;
}waitNode;

typedef struct waitNode* node;

static node waitingList = NULL;

void addToWaitingList(int ticks, int pid);
void removeFromWaitingList(int pid);

char userInterrupted = FALSE;

extern unsigned char clock(unsigned char mode);

/*
 * Primero se extrae la primera cifra multiplicando el valor del 
 * primer nibble (los primeros 4 bits) por 10. 
 * Luego, se extrae la segunda cifra tomando 
 * el valor del segundo nibble (los últimos 4 bits) y 
 * se suma al valor de la primera cifra convertida a decimal. 
 */
static unsigned  int decode(unsigned char time){
    return (time >> 4) * 10 + (time & 0x0F);
    //Decalaje para formato bcd .. b0b1b2b3b4b5b6b7
    // b0b1b2b3  --> Parte decimal alta
    // b4b5b6b7 --> Parte decimal baja --> hago el & para que solo agarre esos bits
}

unsigned int seconds(){
    return decode(clock(SECONDS));
}

unsigned int minutes(){
    return decode(clock(MINUTES));
}

unsigned int hours(){
    return decode(clock(HOURS)) + TIME_ZONE;
}

unsigned int year(){
    return decode(clock(YEAR));
}

unsigned int day(){
    return decode(clock(DAYS));
}

unsigned int month(){
    return decode(clock(MONTH));
}

void timeToStr(char * dest){
    dest[2] = dest[5] = ':';
    uint8_t s, m, h = hours();

    dest[0] = (h/10) % 10 + '0';    //hour 9 ==> deja 0 luego deja el 9
    dest[1] = h % 10 + '0';
    m = minutes();
    dest[3] = (m/10)%10 + '0';      //min 13 ==> deja 1 y luego deja 3
    dest[4] = m % 10 + '0';
    s= seconds();
    dest[6] = (s /10) %10 + '0';
    dest[7] = s % 10 + '0';
}

void dateToStr(char * dest){
    dest[2] = dest[5] = '/';
    uint8_t d = day(),m,y;
    dest[0] = (d/10) + '0';
    dest[1] = d % 10 + '0';
    m = month();
    dest[3] = (m/10) % 10 + '0';
    dest[4] = m%10 + '0';
    y = year();
    dest[6] = (y/10) % 10 + '0';
    dest[7] = y %10 + '0';
}

static unsigned long ticks = 0;

void timer_handler() {
    if(userInterrupted == FALSE) {
        ticks++;
        node curr = waitingList;
        while(curr != NULL) {
            if(curr->remainingTicks <= 0) {
                removeFromWaitingList(curr->pid);
            } else {
                curr->remainingTicks--;
            }
            curr = curr->next;
        }
    }
    userInterrupted = FALSE;
}

int ticks_elapsed() {
    return ticks;
}

int seconds_elapsed() {
    return ticks / 18;
}

void addToWaitingList(int totalTicks, int pid) {
    node n = allocate(sizeof(waitNode));
    n->remainingTicks = totalTicks;
    n->pid = pid;
    n->next = waitingList;
    waitingList = n;
}

void removeFromWaitingList(int pid) {
    node curr = waitingList;
    node prev;
    if(curr != NULL && curr->pid == pid) {
        waitingList = curr->next;
        deallocate(curr);
        unblockProcess(pid);
        return;
    }

    prev = curr;
    curr = curr->next;
    while(curr != NULL && curr->pid != pid) {
        prev = curr;
        curr = curr->next;
    }
    if(curr != NULL) {
        prev->next = curr->next;
        deallocate(curr);
        unblockProcess(pid);
        return;
    }
}

void wait(uint64_t milliseconds) {
    int pid = getActiveProcessPid();
    int totalTicks = (int)(milliseconds / TICK_MS);
    addToWaitingList(totalTicks, pid);
    blockCurrentProcess();
}

void raiseUserInterruptedFlag() {
    userInterrupted = TRUE;
}


/*void wait(uint64_t milliseconds) {
    unsigned long initTicks = ticks;
    while((ticks - initTicks) < (milliseconds / 55)) {

    }
}*/
