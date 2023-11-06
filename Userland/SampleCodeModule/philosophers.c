#include <system.h>
#include <standardLib.h>
#define MAX_PHILOSOPHERS 6
#define MIN_PHILOSOPHERS 3
#define INIT_PHILOSOPHERS 5
#define HUNGRY 0
#define EATING 1
#define THINKING 2
void initPhyloReunion();
//not HEAVILY inspired by tanenbaum at all :P
//                                         ^ AI made it

//aka semaphoreName
char philosophersName [MAX_PHILOSOPHERS][24]={"Aristoteles","Platon","Socrates","Descartes","Kant","Nietzsche"};

void test(int philoNum);
void takeForks(int philoNum);
void putForks(int philoNum);
void eat(int philoNumber);
void think(int philoNumber);

static const char tablemutex[]="TableMutex";
typedef struct philosopher{
    char name[24];
    int state;
    int pid;
}philosopher;

static int philoAmount = 0;
typedef struct philosopher* philo;

static philo philosophers[MAX_PHILOSOPHERS];


void philosopherActivity(int argc,char* argv[]) {
    waitSem(tablemutex);
    int philoNumber=philoAmount;
    philoAmount++;
    postSem(tablemutex);
    while(1) {
        think(philoNumber);
        takeForks(philoNumber);
        eat(philoNumber);
        putForks(philoNumber);
    }
}

void initPhyloReunion(int argc,char* argv[]) {
    destroySem(tablemutex);
    openSem(tablemutex,1);
    philoAmount=0;
    for(int i=0;i<MAX_PHILOSOPHERS;i++) {
        destroySem(philosophersName[i]);
        openSem(philosophersName[i],1);
    }
    waitSem(tablemutex);
    for(int i=0;i<INIT_PHILOSOPHERS;i++) {
        philosophers[i] = alloc(sizeof(philosopher));
        strcpy(philosophers[i]->name,philosophersName[i]);
        philosophers[i]->state = THINKING;
        philosophers[i]->pid = createProcess(philosopherActivity,HIGH,BACKGROUND,0,philosophersName[i],NULL);
    }
    postSem(tablemutex);
    while(1){
        wait(1000);
        waitSem(tablemutex);
        for(int i=0;i<philoAmount;i++){
            if(philosophers[i]->state == EATING)
                printFormat("E");
            else
                printFormat(".");
        }
        printFormat("\n");
        postSem(tablemutex);
    }
    exitProc();
}

void eat(int philoNumber) {
    wait(1200);
}

void think(int philoNumber) {
    wait(3000);
}

void takeForks(int philoNum) {
    waitSem(tablemutex);
    philosophers[philoNum]->state = HUNGRY;
    test(philoNum);
    postSem(tablemutex);
    waitSem(philosophersName[philoNum]);
}

void putForks(int philoNum) {
    waitSem(tablemutex);
    philosophers[philoNum]->state = THINKING;
    test((philoNum + philoAmount - 1) % philoAmount);
    test((philoNum + 1) % philoAmount);
    postSem(tablemutex);
}

void test(int philoNum){
    if((philosophers[philoNum]->state == HUNGRY) && (philosophers[(philoNum + philoAmount - 1) % philoAmount]->state != EATING) && (philosophers[(philoNum + 1) % philoAmount]->state != EATING)) {
        philosophers[philoNum]->state=EATING;
        postSem(philosophersName[philoNum]);
    }
}
