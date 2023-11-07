#include <memoryManager.h>
#include "lib.h"
#include <stddef.h>
#include <console.h>
#include <processManager.h>
#include <videoDriver.h>

#define MEM_START_ADR 0x0000000000050000
#define MIN_SIZE 64   // MIN_SIZE == 64 -> sh phylo works fine after sh loop. 128 <= MIN_SIZE for some reason sh phylo does not work as intended

//Tree styled struct
typedef struct Buddyblock {
    int size;
    char pid;
    char isFree;
    char isSplit;
    struct Buddyblock *left;
    struct Buddyblock *right;
} Buddyblock;

static Buddyblock *root = NULL;

Buddyblock *createBlock(int size, void *address) {
    Buddyblock * block = (Buddyblock *) address;

    block->size = size;
    block->isFree = 1;
    block->isSplit = 0;
    block->left = NULL;
    block->right = NULL;
    block->pid = (char) getActiveProcessPid();

    return block;
}

void createMemoryManager() {
    void * memory = (void *)MEM_START_ADR;
    root = createBlock(MEMORY_SIZE, memory);
}

//Will calculate appropiate size for buddy block
size_t convertToClosestPowerOf2(size_t size) {
    if (size < MIN_SIZE) {
        return size <= MIN_SIZE - sizeof(Buddyblock) ? MIN_SIZE: 2*MIN_SIZE;
    }

    size_t power = 1;
    while (power < size) {
        power <<= 1;
    }

    return power < MIN_SIZE ? MIN_SIZE: power;
}


void splitBlock(Buddyblock *block) {
    if(block->size == MIN_SIZE){
        return;
    }

    block->isSplit = 1;
    block->isFree = 0;
    block->pid = -2;
    block->left = createBlock(block->size / 2, (void*)((char *)block + sizeof(struct Buddyblock)));
    block->right = createBlock(block->size / 2, (void*)((char *)block + block->size/2));
}

//Recursive function call to merge 'buddies' if mergeable as to reduce fragmentation and increase the general memory pool
void mergeBlocksRecursive(Buddyblock *node) {
    if(node == NULL){
        return;
    }

    if (node->isSplit) {
        mergeBlocksRecursive(node->left);
        mergeBlocksRecursive(node->right);

        if (node->left->isFree && node->right->isFree) {
            node->isSplit = 0;
            node->isFree = 1;
            node->left = NULL;
            node->right = NULL;
        }
    }
}

void mergeBlocks() {
    mergeBlocksRecursive(root);
}

//DFS styled function to make the tree grow as more memory is needed.
Buddyblock* allocateRecursive(size_t size, Buddyblock* node) {
    if(node == NULL || (!node->isFree && !node->isSplit)){
        return NULL;
    }

    if(node->isFree && node->size == size){
        node->isFree = 0;
        return node;
    }

    Buddyblock * left;
    Buddyblock * right;

    if(!node->isSplit && node->isFree && node->size/2 >= MIN_SIZE){
        splitBlock(node);
    }

    left = allocateRecursive(size,node->left);
    if(left != NULL){
        return left;
    }

    right = allocateRecursive(size,node->right);
    if(right != NULL){
        return right;
    }

    return NULL;
}

void *allocate(size_t size) {
    if(size >= MEMORY_SIZE){
        return NULL;
    }

    size_t sizeToUse = convertToClosestPowerOf2(size);
    Buddyblock *node = root;
    mergeBlocks();
    Buddyblock *allocatedBlock = allocateRecursive(sizeToUse, node);

    if (allocatedBlock != NULL) {
        return (void*)((char*)allocatedBlock + sizeof(Buddyblock));
    }

    return NULL;
}

void deallocate(void *addr) {
    if(addr == NULL){
        return;
    }

    if((uintptr_t)addr < 0x50000 || (uintptr_t)addr > 0x60000){
        cPrintColored(RED,"Illegal Memory free");
        return;
    }
    Buddyblock *block = (Buddyblock *)((char *)addr - sizeof(Buddyblock));
    block->isFree = 1;
    block->pid = -2;

    mergeBlocks();
}

void * reallocate(void * ptr, size_t newSize) {
    if (newSize == 0) {
        deallocate(ptr);
        return NULL;
    }

    if (ptr == NULL) {
        return allocate(newSize);
    }

    void * newPtr = allocate(newSize);

    if (newPtr) {
        size_t oldSize = ((Buddyblock *)(ptr - sizeof(Buddyblock)))->size;
        size_t copySize = (oldSize < newSize) ? oldSize : newSize;
        memcpy(newPtr, ptr, copySize);

        deallocate(ptr);
        return newPtr;
    } else {
        return NULL;
    }
}

//Recursive function call to go over the whole tree in order to check free memory and sum it up
size_t getCurrentFreeMemRecursively(Buddyblock * node){
    if (node == NULL) {
        return 0;
    }

    if (node->isFree) {
        return node->size - sizeof(Buddyblock);
    }

    if (node->isSplit) {
        size_t leftSize = getCurrentFreeMemRecursively(node->left);
        size_t rightSize = getCurrentFreeMemRecursively(node->right);

        return leftSize + rightSize;
    } else {
        return 0;
    }
}

size_t getCurrentMemSize(){
    return getCurrentFreeMemRecursively(root);
}

//Recursive function call to go over the whole tree in order to check pids and dealloc if needed
void deallocateAllProcRecursive(Buddyblock * node, int pid){
    if (node == NULL) {
        return;
    }

    if (node->isSplit) {
        deallocateAllProcRecursive(node->left, pid);
        deallocateAllProcRecursive(node->right, pid);
    } else if (!node->isFree && node->pid == pid) {
        node->isFree = 1;
        node->pid = -1;
    }
}

void deallocateAllProcessRelatedMem(int pid){
    deallocateAllProcRecursive(root,pid);
    mergeBlocks();
}