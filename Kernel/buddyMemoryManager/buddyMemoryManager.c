#include <memoryManager.h>
#include "lib.h"
#include <stddef.h>
#include <console.h>
#include <processManager.h>
//64 KB libres
#define MEM_START_ADR 0x0000000000050000
#define MIN_SIZE 512

typedef struct BuddyBlock {
    int size;
    char pid;
    char isFree;
    char isSplit;
    struct BuddyBlock *left;
    struct BuddyBlock *right;
} BuddyBlock;

static BuddyBlock *root = NULL;

static size_t currentAvailableMemory;

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
        return size;
    }

    size_t power = 1;
    while (power < size) {
        power <<= 1;
    }

    return power;
}

void splitBlock(BuddyBlock *block) {
    if(block->size <= MIN_SIZE){
        return;
    }

    block->isSplit = 1;
    block->left = createBlock(block->size / 2, (void*)block + sizeof(struct BuddyBlock));
    block->right = createBlock(block->size / 2, (void*)block + block->size / 2);

    currentAvailableMemory -= 2*sizeof(BuddyBlock);
}

BuddyBlock* allocateRecursive(size_t size, BuddyBlock* node) {

    if (node == NULL || (!node->isFree) || node->size < size) {
        return NULL;
    }

    if (node->size == size && !node->isSplit) {
        node->isFree = 0;
        return node;
    }

    if(!node->isSplit) {
        splitBlock(node);
    }

    BuddyBlock* leftAlloc = allocateRecursive(size, node->left);
    if (leftAlloc != NULL) {
        return leftAlloc;
    }

    BuddyBlock* rightAlloc = allocateRecursive(size, node->right);
    if (rightAlloc != NULL) {
        return rightAlloc;
    }

    node->isFree = 0;

    return node;
}

void *allocate(size_t size) {
    size_t sizeToUse = convertToClosestPowerOf2(size);
    BuddyBlock *node = root;

    BuddyBlock *allocatedBlock = allocateRecursive(sizeToUse, node);

    if (allocatedBlock != NULL) {
        currentAvailableMemory -= sizeof(struct BuddyBlock) + allocatedBlock->size;
        return (void *)allocatedBlock + sizeof(struct BuddyBlock);
    }


    return NULL;
}

void mergeBlocksRecursive(BuddyBlock *node) {
    if (node->isSplit) {
        if (node->left->isFree && node->right->isFree) {
            node->isSplit = 0;
            node->left = NULL;
            node->right = NULL;
            currentAvailableMemory += node->size + 2*sizeof(BuddyBlock);;
        } else {
            if (node->left->isSplit) {
                mergeBlocksRecursive(node->left);
            }
            if (node->right->isSplit) {
                mergeBlocksRecursive(node->right);
            }
        }
    }
}

void mergeBlocks() {
   mergeBlocksRecursive(root);
}


void deallocate(void *addr) {
    BuddyBlock *block = (BuddyBlock *)((uintptr_t*)addr - sizeof(struct BuddyBlock));
    currentAvailableMemory += block->size + sizeof(struct BuddyBlock);
    block->isFree = 1;

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

size_t getCurrentMemSize(){
    return currentAvailableMemory;
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