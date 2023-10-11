#include "include/memoryManager.h"
#include <stdio.h>
#include "include/processADT.h"

typedef struct node{
    processADT proc;
    node * next;
}node;

static node * highPriority = NULL;
static node * midPriority = NULL;
static node * lowPriority = NULL;


void roundRobin(){
    
}


node * initialize(processADT p){
    node new;
    new.proc = p;
    new.next = highPriority;

    return &new;
}

