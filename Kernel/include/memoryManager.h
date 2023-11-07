#include <stddef.h>
#define PAGE_SIZE 256
#define MEMORY_SIZE 65536

//La memoria se definio en el .c pues donde arranca y termina la memoria no deberia ser visible para tod el so, sino para el MM

/**
 * Initializes memoryManager
 * Sets firstBlock to  0x0000000000050000
 * Sets size to MEMORY_SIZE
 */
void createMemoryManager();

/**
 *  Allocates the given size and reserves that memory block.
 * @return pointer to the memory block
 */

void * allocate(size_t size);
/**
 * Deallocates memory on the given address and free's the block.
 * @param addr
 */
void deallocate(void * addr);

/**
 * Allocates a block with the new size, copies the old block content and free's the old block.
 * @param ptr
 * @param newSize
 * @return
 */
void * reallocate(void * ptr, size_t newSize);

/**
 * Goes through the list and checks how much free space remains
 * @return available space
 */
size_t getCurrentMemSize();

/**
 * Deallocates all memory with a given pid
 * @param pid
 */
void deallocateAllProcessRelatedMem(int pid);