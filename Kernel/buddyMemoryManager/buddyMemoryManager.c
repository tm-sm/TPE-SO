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

static BlockHeader* free_lists[MAX_LEVEL];
static BlockHeader* first_block;

void createMemoryManager() {
    for (int i = 0; i < MAX_LEVEL; i++) {
        free_lists[i] = NULL;
    }

    first_block = (BlockHeader*)MEM_START_ADR;
    first_block->size = MEMORY_SIZE;
    first_block->is_free = 1;
    first_block->next = NULL;

    free_lists[MAX_LEVEL-1] = first_block;
}

void splitBlock(BlockHeader* block, int level) {

}

void mergeBuddies(BlockHeader* block, int level) {

}

BlockHeader* findFreeBlock(int level) {

}

void* allocate(size_t size) {

}

void deallocate(void* addr) {

}

void* reallocate(void* ptr, size_t newSize) {

}

size_t convertToPageSize(size_t size, size_t pageSize){
    if (pageSize == 0) {
        return 0;
    }

    size_t quotient = size / pageSize;

    return pageSize*quotient + pageSize;
}