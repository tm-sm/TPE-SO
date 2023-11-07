#include <stddef.h>
#define PAGE_SIZE 256
#define MEMORY_SIZE 65536

/**
 * Initializes memoryManager
 * Sets firstBlock to  0x0000000000050000
 * Sets size to MEMORY_SIZE
 */
void createMemoryManager();

/**
 *  Allocates the given size and reserves that memory block.
 * Returns a pointer to the memory block
 */

void* allocate(size_t size);
/**
 * Deallocates memory on the given address and free's the block.
 */
void deallocate(void* addr);

/**
 * Allocates a block with the new size, copies the old block content and free's the old block.
 */
void* reallocate(void* ptr, size_t newSize);

/**
 * Goes through the list and checks how much free space remains. It returns the sum of it.
 */
size_t getCurrentMemSize();

/**
 * Deallocates all memory with a given pid
 */
void deallocateAllProcessRelatedMem(int pid);