#define size_t unsigned long int
#define PAGE_SIZE 512

//La memoria se definio en el .c pues donde arranca y termina la memoria no deberia ser visible para tod el so, sino para el MM

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict memForMM, void *const restrict memToManage);
void *allocMemory(MemoryManagerADT const restrict memManager, const size_t memBlockSize);
