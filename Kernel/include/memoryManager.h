#include <stddef.h>
#define PAGE_SIZE 256
#define MEMORY_SIZE 65536

//La memoria se definio en el .c pues donde arranca y termina la memoria no deberia ser visible para tod el so, sino para el MM

void createMemoryManager();

void * allocate(size_t size);

void deallocate(void * addr);

void * reallocate(void * ptr, size_t newSize);

size_t convertToPageSize(size_t size, size_t pageSize);

size_t getCurrentMemSize();

void deallocateAllProcessRelatedMem(int pid);