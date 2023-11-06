#include <system.h>
#include <standardLib.h>
#define MAX_PHILOSOPHERS 6
#define MIN_PHILOSOPHERS 3
#define INIT_PHILOSOPHERS 5
#define HUNGRY 0
#define EATING 1

void initPhyloReunion();
//not HEAVILY inspired by tanenbaum at all :P
//                                         ^ AI made it

//aka semaphoreName
char philosophersName [MAX_PHILOSOPHERS][24]={"Aristoteles","Platon","Socrates","Descartes","Kant","Nietzsche"};

const char * tablemutex="TableMutex";
typedef struct philosopher{
    char name[24];
    int state;
    int pid;
}philosopher;

static int philoAmount=0;
typedef struct philosopher* philo;

philo philosophers[MAX_PHILOSOPHERS];




void philosopherActivity(){
    waitSem(tablemutex);
    int philoNumber=philoAmount;
    philoAmount++;
    postSem(tablemutex);
    while(1){
        takeForks(philoNumber);
        philosophers[philoNumber]->state=EATING;
        putForks(philoNumber);
    }
}

void initPhyloReunion(int argc,char** argv){
    openSem(tablemutex,1);
    for(int i=0;i<MAX_PHILOSOPHERS;i++){
        openSem(philosophersName[i],1);
    }
    waitSem(tablemutex);
    for(int i=0;i<INIT_PHILOSOPHERS;i++){
        strcpy(philosophers[i]->name,philosophersName[i]);
        philosophers[i]->state=HUNGRY;
        philosophers[i]->pid=createProcess(philosopherActivity,HIGH,BACKGROUND,0,philosophersName[i],NULL);
    }
    postSem(tablemutex);
    while(1){
        for(int i=0;i<INIT_PHILOSOPHERS;i++){
            if(philosophers[i]->state==EATING)
                printFormat("E");
            else
                printFormat(".");
        }
        printFormat("\n");
        wait(1000);
    }
}

void takeForks(int i){
    waitSem(tablemutex);
    test(i);
    postSem(tablemutex);
    waitSem(philosophersName[i]);
}

void putForks(int i){
    waitSem(tablemutex);
    philosophers[i]->state=HUNGRY;
    test(i%philoAmount);
    test((i+1)%philoAmount);
    postSem(tablemutex);
}

void test(int i){
    if( philosophers[i]->state!=EATING && philosophers[(i+philoAmount-1)%philoAmount]->state !=EATING && philosophers[(i+1)%philoAmount]->state!=EATING){
        philosophers[i]->state=EATING;
        postSem(philosophersName[i]);
    }
}


