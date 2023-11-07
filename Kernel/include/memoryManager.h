#include <stddef.h>
#define PAGE_SIZE 256
#define MEMORY_SIZE 65536

//La memoria se definio en el .c pues donde arranca y termina la memoria no deberia ser visible para tod el so, sino para el MM

/**
 * Initializes memoryManager
 * Sets firstBlock to  0x0000000000050000
 *
 */
void createMemoryManager();

/**
 *
 * @param size
 * @return
 */
void * allocate(size_t size);
/**
 *
 * @param addr
 */
void deallocate(void * addr);
/**
 *
 * @param ptr
 * @param newSize
 * @return
 */
void * reallocate(void * ptr, size_t newSize);

/**
 *
 * @return
 */
size_t getCurrentMemSize();

/**
 *
 * @param pid
 */
void deallocateAllProcessRelatedMem(int pid);