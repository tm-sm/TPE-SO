#include <memoryManager.h>
#include <processManager.h>
#include "lib.h"
#define MEM_START_ADR 0x0000000000050000

typedef struct BlockHeader {
    size_t size;
    size_t isFree;
    int pid;
    struct BlockHeader *next;
}BlockHeader;

/*
 * Struct -> Define block with the following data:
 * space occupied.
 * if it is in use.
 * Pointer to the next one in the list.
 */

//Static pointer to the beginning of memmory
static BlockHeader * firstBlock;

void createMemoryManager() {
    // Initialize the block header making reference to the beginning of the memory array

    firstBlock = (BlockHeader *)MEM_START_ADR;
    firstBlock->size = MEMORY_SIZE - sizeof(BlockHeader);
    firstBlock->isFree = 1;
    firstBlock->pid = getActiveProcessPid();
    firstBlock->next = NULL;
}

void * allocate(size_t size) {
    if(size > MEMORY_SIZE || size >= getCurrentMemSize()){
        return NULL;
    }

    size_t pageTBU = convertToPageSize(size, PAGE_SIZE);
    // Now i should look for a block with enough space
    BlockHeader* currentBlock = firstBlock;
    while (currentBlock) {
        if (currentBlock->isFree && currentBlock->size >= pageTBU) {

            // If the block is bigger than needed, save the space and leave the extra free;
            if (currentBlock->size >= pageTBU + sizeof(BlockHeader)) {

                BlockHeader* newBlock = (BlockHeader*)((char*)currentBlock + sizeof(BlockHeader) + pageTBU);    // Pointer to new block in position currentBlock + struct size + ammount of memory to be saved

                // newBlock reduce its size in size and sizeof(BlockHeader), starts free, and next as next of current block
                newBlock->size = currentBlock->size - pageTBU - sizeof(BlockHeader);
                newBlock->isFree = 1;
                newBlock->next = currentBlock->next;


                //current block gets the required size, saves it and next is the new block
                currentBlock->size = pageTBU;
                currentBlock->isFree = 0;
                currentBlock->next = newBlock;
                currentBlock->pid = getActiveProcessPid();
            } else {  //got just the space for the block => keep the space
                currentBlock->isFree = 0;
            }


            // return pointer to that section of the block
            return (void*)((char*)currentBlock + sizeof(BlockHeader));
        }
        // contine to the next block if there is not enough space
        currentBlock = currentBlock->next;
    }

    // No memory blocks with enough space
    return NULL;
}

void deallocate(void * ptr) {
    if (ptr == NULL) {
        return;  // if it is pointing to nothing => there is noting to free
    }


    if ((size_t) ptr < MEM_START_ADR || (size_t) ptr >= (MEM_START_ADR + MEMORY_SIZE)) {
        return;
    }

    // get pointer to the block i want to free y change the flag isFree to 1
    BlockHeader * block = (BlockHeader *)((char *)ptr - sizeof(BlockHeader));
    block->isFree = 1;


    // Combine consecutive free block
    BlockHeader * currentBlock = firstBlock;    // Start from the beginning
    while (currentBlock) {
        if (currentBlock->isFree) {
            if (currentBlock->next && currentBlock->next->isFree) {
                currentBlock->size += sizeof(BlockHeader) + currentBlock->next->size;
                currentBlock->next = currentBlock->next->next;
            }
        }
        // Move on to the next
        currentBlock = currentBlock->next;
    }
}

void deallocateAllProcessRelatedMem(int pid){
    BlockHeader * current = firstBlock;

    while(current != NULL){
        if(current->pid == pid){
            deallocate(current + sizeof(BlockHeader));
        }

        current = current->next;
    }
}

void * reallocate(void * ptr, size_t newSize){
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
        size_t oldSize = ((BlockHeader *)(ptr - sizeof(BlockHeader)))->size;
        size_t copySize = (oldSize < newSize) ? oldSize : newSize;  //Copy the minimum to not go over on, CAN CAUSE MEM LEAK
        memcpy(newPtr, ptr, copySize);

        deallocate(ptr);

        return newPtr;
    } else {
        return NULL;
    }
}

size_t convertToPageSize(size_t size, size_t pageSize){
    if (pageSize == 0) {
        return 0;
    }

    size_t quotient = size / pageSize;

    return pageSize*quotient + pageSize;
}

size_t getCurrentMemSize(){
    size_t currentAvailable = 0;

    BlockHeader * current = firstBlock;

    while(current){
        if(current->isFree){
            currentAvailable+=current->size;
        }
        current = current->next;
    }

    return currentAvailable;
}
