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

static const char tablemutex[]="TableMutex";
typedef struct philosopher{
    char name[24];
    int state;
    int pid;
}philosopher;

static int philoAmount=INIT_PHILOSOPHERS;
typedef struct philosopher* philo;

static philo philosophers[MAX_PHILOSOPHERS];


void philosopherActivity(int argc,char* argv[]){
    waitSem(tablemutex);
    int philoNumber=philoAmount;
    postSem(tablemutex);
    while(1){
        takeForks(philoNumber);
        philosophers[philoNumber]->state=EATING;
        putForks(philoNumber);
    }
}

void initPhyloReunion(int argc,char* argv[]){
    openSem(tablemutex,1);
    philoAmount=0;
    for(int i=0;i<MAX_PHILOSOPHERS;i++){
        openSem(philosophersName[i],1);
    }
    waitSem(tablemutex);
    for(int i=0;i<INIT_PHILOSOPHERS;i++){
        strcpy(philosophers[i]->name,philosophersName[i]);
        philosophers[i]->state=HUNGRY;
        philosophers[i]->pid=createProcess(philosopherActivity,HIGH,BACKGROUND,0,philosophersName[i],NULL);
        philoAmount++;
    }
    postSem(tablemutex);
    while(1){
        waitSem(tablemutex);
        for(int i=0;i<philoAmount;i++){
            if(philosophers[i]->state==EATING)
                printFormat("E");
            else
                printFormat(".");
        }
        printFormat("\n");
        wait(1000);
        postSem(tablemutex);
    }
}

void takeForks(int philoNum){
    waitSem(tablemutex);
    philosophers[philoNum]->state=HUNGRY;
    test(philoNum);
    postSem(tablemutex);
    waitSem(philosophersName[philoNum]);
}

void putForks(int philoNum){
    waitSem(tablemutex);
    philosophers[philoNum]->state=THINKING;
    test(philoNum%philoAmount);
    test((philoNum+1)%philoAmount);
    postSem(tablemutex);
}

void test(int philoNum){
    if(philosophers[(philoNum+philoAmount-1)%philoAmount]->state !=EATING && philosophers[(philoNum+1)%philoAmount]->state!=EATING){
        philosophers[philoNum]->state=EATING;
        postSem(philosophersName[philoNum]);
    }
}


