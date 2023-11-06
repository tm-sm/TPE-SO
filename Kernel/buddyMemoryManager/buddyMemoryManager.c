#include <memoryManager.h>
#include "lib.h"
#include <stddef.h>
#include <console.h>
#include <processManager.h>
#include <videoDriver.h>

//64 KB libres
#define MEM_START_ADR 0x0000000000050000
#define MIN_SIZE 256

typedef struct BuddyBlock {
    int size;
    char pid;
    char isFree;
    char isSplit;
    struct BuddyBlock *left;
    struct BuddyBlock *right;
} BuddyBlock;

static BuddyBlock *root = NULL;

BuddyBlock *createBlock(int size, void *address) {
    BuddyBlock * block = (BuddyBlock *) address;

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

size_t convertToClosestPowerOf2(size_t size) {
    if ((size & (size - 1)) == 0) {
        return size < MIN_SIZE ? MIN_SIZE: size;
    }

    size_t power = 1;
    while (power < size) {
        power <<= 1;
    }

    return power < MIN_SIZE ? MIN_SIZE: power;
}

void splitBlock(BuddyBlock *block) {
    if(block->size == MIN_SIZE){
        return;
    }

    block->isSplit = 1;
    block->isFree = 0;
    block->pid = -1;
    block->left = createBlock(block->size / 2, (void*)((char *)block + sizeof(struct BuddyBlock)));
    block->right = createBlock(block->size / 2, (void*)((char *)block + block->size/2));
}

void mergeBlocksRecursive(BuddyBlock *node) {
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

//El size ya es el size que se va reservar por el sistema
BuddyBlock* allocateRecursive(size_t size, BuddyBlock* node) {
    if(node == NULL || (!node->isFree && !node->isSplit)){
        return NULL;
    }

    if(node->isFree && node->size == size){
        node->isFree = 0;
        return node;
    }

    BuddyBlock * left;
    BuddyBlock * right;

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
    if(size >= MEMORY_SIZE || size >= getCurrentMemSize()){
        return NULL;
    }

    size_t sizeToUse = convertToClosestPowerOf2(size);
    BuddyBlock *node = root;
    mergeBlocks();
    BuddyBlock *allocatedBlock = allocateRecursive(sizeToUse, node);

    if (allocatedBlock != NULL) {
        return (void*)((char*)allocatedBlock + sizeof(BuddyBlock));
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
   BuddyBlock *block = (BuddyBlock *)((char *)addr - sizeof(BuddyBlock));
   block->isFree = 1;
   block->pid = -1;
    mergeBlocks();
}

void * reallocate(void * ptr, size_t newSize) {
    //Equivalente a un dealloc
    if (newSize == 0) {
        deallocate(ptr);
        return NULL;
    }
    //Si no hay nada, es como un allocate
    if (ptr == NULL) {
        return allocate(newSize);
    }

    void * newPtr = allocate(newSize);

    if (newPtr) {
        size_t oldSize = ((BuddyBlock *)(ptr - sizeof(BuddyBlock)))->size;
        size_t copySize = (oldSize < newSize) ? oldSize : newSize;  //Copio lo minimo para no excederme del tamano, ESTO PUEDE CAUSAR PERDIDAS DE MEMORIA
        memcpy(newPtr, ptr, copySize);

        deallocate(ptr);
        return newPtr;
    } else {
        return NULL;
    }
}

size_t getCurrentFreeMemRecursively(BuddyBlock * node){
    if (node == NULL) {
        return 0;
    }

    if (node->isFree) {
        return node->size - sizeof(BuddyBlock);
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

size_t convertToPageSize(size_t size, size_t pageSize) {
    if (pageSize == 0) {
        return 0;
    }

    size_t quotient = size / pageSize;

    return pageSize*quotient + pageSize;
}

void deallocateAllProcRecursive(BuddyBlock * node, int pid){
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