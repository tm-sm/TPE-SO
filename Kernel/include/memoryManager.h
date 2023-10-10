#include <stddef.h>
#define PAGE_SIZE 512

//La memoria se definio en el .c pues donde arranca y termina la memoria no deberia ser visible para tod el so, sino para el MM

void createMemoryManager();

void * allocate(size_t size);

void deallocate(void * addr);

