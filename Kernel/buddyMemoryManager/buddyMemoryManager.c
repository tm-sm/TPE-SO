#include <memoryManager.h>
#include "lib.h"
#include <stddef.h>
#include <console.h>
//64 KB libres
#define MEM_START_ADR 0x0000000000050000
#define MIN_SIZE 512

typedef struct BuddyBlock {
    int size;
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
    /*
    cPrint("Created Mem block addr: ");
    cPrintHex((uint64_t) block);
    cNewline();
    cPrint("Size: ");
    cPrintDec(block->size);
    cNewline();
     */
    return block;
}

void createMemoryManager() {
    void * memory = (void *)MEM_START_ADR;
    root = createBlock(MEMORY_SIZE, memory);

}

size_t convertToClosestPowerOf2(size_t size) {
    // If the size is already a power of 2, return it
    if ((size & (size - 1)) == 0) {
        return size;
    }

    size_t power = 1;
    while (power < size) {
        power <<= 1;
    }

    return power;
}


// Right offset = size/2 , left offset = sizeOf(BuddyBlock) para preservar punteros anteriores
void splitBlock(BuddyBlock *block) {
    if(block->size <= MIN_SIZE){
        return;
    }

    block->isSplit = 1;
    block->left = createBlock(block->size / 2, (void*)block + sizeof(struct BuddyBlock));
    block->right = createBlock(block->size / 2, (void*)block + block->size / 2);
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
    /*
    cPrint("Requested size: ");
    cPrintDec(size);
    cNewline();
    */
    size_t sizeToUse = convertToClosestPowerOf2(size);
    BuddyBlock *node = root;

    // Find a suitable block
    BuddyBlock *allocatedBlock = allocateRecursive(sizeToUse, node);
    /*
    cPrint("Returning: ");
    cPrintHex((uint64_t)allocatedBlock);
    cNewline();
    */
    if (allocatedBlock != NULL) {
        return (void *)allocatedBlock + sizeof(struct BuddyBlock);
    }


    // No suitable block found
    return NULL;
}

void mergeBlocks(BuddyBlock **node) {
    if (*node == NULL) {
        return;
    }

    BuddyBlock *parent = NULL;

    if (*node == (*node)->left) {
        parent = (*node)->right;
    } else {
        parent = (*node)->left;
    }

    if (parent != NULL && parent->isFree && !parent->isSplit) {
        (*node)->isFree = 1;
        (*node)->isSplit = 0;
        (*node)->left = NULL;
        (*node)->right = NULL;
        (*node)->size *= 2;

        mergeBlocks(&parent);

        *node = parent;
    }
}

void deallocate(void *addr) {
    BuddyBlock *block = (BuddyBlock *)((uintptr_t)addr - sizeof(struct BuddyBlock));

    block->isFree = 1;

    mergeBlocks(&block);
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

size_t convertToPageSize(size_t size, size_t pageSize) {
    if (pageSize == 0) {
        return 0;
    }

    size_t quotient = size / pageSize;

    return pageSize*quotient + pageSize;
}