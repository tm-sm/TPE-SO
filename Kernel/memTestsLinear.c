#include <memTests.h>
#include <memoryManager.h>
#include <console.h>

int memoryAllocTest(){
    char * mem1 = allocate(500);
    char * mem2 = allocate(2603);

    if((uint64_t)mem1 == 0x0000000000050018 && (uint64_t)mem2 == 0x0000000000050230
    && ((BlockHeader *)(mem1 - sizeof(BlockHeader)))->size == convertToPageSize(500,PAGE_SIZE)
    && ((BlockHeader *)(mem2 - sizeof(BlockHeader)))->size == convertToPageSize(2603,PAGE_SIZE)){

        cPrintBase(((BlockHeader *)(mem1 - sizeof(BlockHeader)))->size,10);
        cNewline();
        cPrintBase(((BlockHeader *)(mem2 - sizeof(BlockHeader)))->size,10);
        cNewline();

        cPrintHex((uint64_t)mem1);
        cNewline();
        cPrintHex((uint64_t)mem2);
        cNewline();

        return 1;
    }

    deallocate(mem1);
    deallocate(mem2);
   return 0;
}

int memoryDeallocTest(){
    char * mem3 = allocate(60000);
    cPrintBase(((BlockHeader *)(mem3 - sizeof(BlockHeader)))->size,10);
    cNewline();
    deallocate(mem3);
}

int memoryProperInitSelfCheck(){
    createMemoryManager();
    memoryAllocTest();
    memoryDeallocTest();
}
