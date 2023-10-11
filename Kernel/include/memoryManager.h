#include <stddef.h>
#define PAGE_SIZE 512
#define MEMORY_SIZE 65536

//La memoria se definio en el .c pues donde arranca y termina la memoria no deberia ser visible para tod el so, sino para el MM

typedef struct BlockHeader {
    size_t size;
    size_t is_free;
    struct BlockHeader *next;
}BlockHeader;

void createMemoryManager();

void * allocate(size_t size);

void deallocate(void * addr);

void * realloc(void * ptr, size_t newSize);

size_t convertToPageSize(size_t size, size_t pageSize);
