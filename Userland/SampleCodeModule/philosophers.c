#include <system.h>
#include <standardLib.h>
#define MAX_PHILOSOPHERS 7
#define MIN_PHILOSOPHERS 3
#define INIT_PHILOSOPHERS 5
#define HUNGRY 0
#define EATING 1
#define THINKING 2

#define MUTEX "phyloMutex"

void initPhyloReunion();
//not HEAVILY inspired by tanenbaum at all :P
//                                         ^ AI made it

//aka semaphoreName
char philosophersName [MAX_PHILOSOPHERS][24]={"Aristoteles","Platon","Socrates","Descartes","Kant","Nietzsche","Sartre","Russell","Machiavelli"};

void getInput();
void addPhilo(int i);
void test(int philoNum);
void takeForks(int philoNum);
void putForks(int philoNum);
void eat(int philoNumber);
void think(int philoNumber);

typedef struct Philosopher {
    char name[24];
    int state;
    int pid;
}Philosopher;

static int philoAmount = 0;
typedef struct Philosopher* philo;

static philo philosophers[MAX_PHILOSOPHERS];


void philosopherActivity(int argc, char* argv[]) {
    waitSem(MUTEX);
    int philoNumber=philoAmount;
    philoAmount++;
    postSem(MUTEX);
    while(1) {
        think(philoNumber);
        takeForks(philoNumber);
        eat(philoNumber);
        putForks(philoNumber);
    }
}

void initPhyloReunion() {
    destroySem(MUTEX);
    openSem(MUTEX,1);
    philoAmount=0;
    for(int i=0;i<MAX_PHILOSOPHERS;i++) {
        destroySem(philosophersName[i]);
        openSem(philosophersName[i],1);
    }
    waitSem(MUTEX);
    for(int i=0;i<INIT_PHILOSOPHERS;i++) {
        addPhilo(i);
    }
    postSem(MUTEX);
    while(1) {
        getInput();
        wait(500);
        waitSem(MUTEX);
        for(int i=0;i<philoAmount;i++) {
            if(philosophers[i]->state == EATING) {
                printFormat("E");
            }
            else {
                printFormat(".");
            }
        }
        printFormat("\n");
        postSem(MUTEX);
    }
}

void addPhilo(int i) {
    philosophers[i] = alloc(sizeof(Philosopher));
    if(philosophers[i]==NULL) {
        printFormat("Error creating Philosopher %s\n",philosophersName[i]);
        return -1;
    }
    strcpy(philosophers[i]->name,philosophersName[i]);
    philosophers[i]->state = THINKING;
    int pid= createProcess(philosopherActivity,HIGH,BACKGROUND,0,philosophersName[i],NULL);
    if(pid==-1) {
        printFormat("Error creating Philosopher %s\n",philosophersName[i]);
    }
    else {
        philosophers[i]->pid=pid;
    }
}

void getInput() {
    waitSem(MUTEX);
    if(isCharPressed('r')) {
        if(philoAmount > MIN_PHILOSOPHERS) {
        int pid = philosophers[philoAmount - 1]->pid;
        philoAmount--;
        killProcess(pid);
        }
        else {
            printFormat("Minimum philosophers reached\n");
        }
    } else if(isCharPressed('a')) {
        if(philoAmount < MAX_PHILOSOPHERS) {
            addPhilo(philoAmount);
        }
        else {
            printFormat("Max philosophers reached\n");
        }
    }
    postSem(MUTEX);
}

void eat(int philoNumber) {
    wait(55 * (1+ philoNumber));
}

void think(int philoNumber) {
    wait(110 * (1+ philoNumber));
}

void takeForks(int philoNum) {
    waitSem(MUTEX);
    philosophers[philoNum]->state = HUNGRY;
    test(philoNum);
    postSem(MUTEX);
    waitSem(philosophersName[philoNum]);
}

void putForks(int philoNum) {
    waitSem(MUTEX);
    philosophers[philoNum]->state = THINKING;
    test((philoNum + philoAmount - 1) % philoAmount);
    test((philoNum + 1) % philoAmount);
    postSem(MUTEX);
}

void test(int philoNum){
    if((philosophers[philoNum]->state == HUNGRY) && (philosophers[(philoNum + philoAmount - 1) % philoAmount]->state != EATING) && (philosophers[(philoNum + 1) % philoAmount]->state != EATING)) {
        philosophers[philoNum]->state=EATING;
        postSem(philosophersName[philoNum]);
    }
}
