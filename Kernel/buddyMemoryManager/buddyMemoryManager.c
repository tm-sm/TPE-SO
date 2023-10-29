#include <memoryManager.h>
#include "lib.h"

//64 KB libres
#define MEM_START_ADR 0x0000000000050000
#define MAX_LEVEL 16

typedef struct BlockHeader {
    size_t size;
    size_t is_free;
    struct BlockHeader* next;
} BlockHeader;

//Array de bloques libres de x tamano
static BlockHeader* free_lists[MAX_LEVEL];

void createMemoryManager() {
    for (int i = 0; i < MAX_LEVEL; i++) {
        free_lists[i] = NULL;
    }

    BlockHeader* initial_block = (BlockHeader*)MEM_START_ADR;
    initial_block->size = MEMORY_SIZE;
    initial_block->is_free = 1;
    initial_block->next = NULL;

    free_lists[MAX_LEVEL - 1] = initial_block;
}

// Parte los bloques
void splitBlock(BlockHeader* block, int level) {
    size_t new_size = 1 << (MAX_LEVEL - level - 1);
    BlockHeader* buddy = (BlockHeader*)((char*)block + new_size);
    buddy->size = new_size;
    buddy->is_free = 1;
    buddy->next = free_lists[level];
    free_lists[level] = buddy;
}

void mergeBuddies(BlockHeader* block, int level) {
    size_t buddy_index = ((char*)block - (char*)MEM_START_ADR) ^ block->size;
    BlockHeader* buddy = (BlockHeader*)((char*)block + buddy_index);

    while (level < MAX_LEVEL - 1 && buddy->is_free && buddy->size == block->size) {
        if (free_lists[level] == buddy) {
            free_lists[level] = buddy->next;
        } else {
            BlockHeader* current = free_lists[level];
            while (current && current->next != buddy) {
                current = current->next;
            }
            if (current && current->next == buddy) {
                current->next = buddy->next;
            }
        }

        block = (buddy_index < ((char*)block - (char*)MEM_START_ADR)) ? buddy : block;
        block->size *= 2;
        level++;
        buddy_index = ((char*)block - (char*)MEM_START_ADR) ^ block->size;
        buddy = (BlockHeader*)((char*)block + buddy_index);
    }

    block->next = free_lists[level];
    free_lists[level] = block;
}

BlockHeader* findFreeBlock(int level) {
    for (int i = level; i < MAX_LEVEL; i++) {
        if (free_lists[i] != NULL) {
            BlockHeader* block = free_lists[i];
            free_lists[i] = block->next;
            return block;
        }
    }
    return NULL;
}

void* allocate(size_t size) {
    if (size == 0 || size > MEMORY_SIZE) {
        return NULL;
    }


    size_t sizeToUse= convertToPageSize(size,PAGE_SIZE);

    int level = 0;
    while ((1 << level) < sizeToUse) {
        level++;
        if (level >= MAX_LEVEL) {
            return NULL;
        }
    }

    BlockHeader* block = findFreeBlock(level);
    if (block == NULL) {
        for (int i = level + 1; i < MAX_LEVEL; i++) {
            block = findFreeBlock(i);
            if (block != NULL) {

                while (i > level) {
                    i--;
                    splitBlock(block, i);
                }
                break;
            }
        }
    }

    if (block != NULL) {
        block->is_free = 0;

        mergeBuddies(block, level);

        return (void*)(block + 1);
    } else {
        return NULL;
    }
}

void deallocate(void* addr) {
    if (addr == NULL) {
        return;
    }

    BlockHeader* block = (BlockHeader*)addr - 1;
    if (block->is_free == 1) {
        return;
    }

    int level = 0;
    while ((1 << level) < block->size) {
        level++;
    }

    block->is_free = 1;
    block->next = free_lists[level];
    free_lists[level] = block;

    mergeBuddies(block, level);
}

void* reallocate(void* ptr, size_t newSize) {
    if (newSize == 0 || newSize > MEMORY_SIZE) {
        return NULL;
    }

    if (ptr == NULL) {
        return allocate(newSize);
    }

    BlockHeader* oldBlock = (BlockHeader*)ptr - 1;
    if (newSize <= oldBlock->size) {
        return ptr;
    }

    void* newBlock = allocate(newSize);
    if (newBlock == NULL) {
        return NULL;
    }

    size_t copySize = (newSize < oldBlock->size) ? newSize : oldBlock->size;
    memcpy(newBlock, ptr, copySize);

    deallocate(ptr);

    return newBlock;
}

size_t convertToPageSize(size_t size, size_t pageSize){
    if (pageSize == 0) {
        return 0;
    }

    size_t quotient = size / pageSize;

    return pageSize*quotient + pageSize;
}