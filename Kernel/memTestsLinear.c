#include <memTests.h>
#include <memoryManager.h>
#include <console.h>

static char * mem1;
static char * mem2;

int memoryAllocTest(){
    mem1 = allocate(500);
    mem2 = allocate(2603);

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

   return 0;
}

int memoryDeallocTest(){

}

int memoryFragmentationTest(){

}

int memoryProcTableNdPagesTest(){

}

int memoryProperInitSelfCheck(){
    createMemoryManager();

    memoryAllocTest();
    memoryDeallocTest();
    memoryFragmentationTest();
    memoryProcTableNdPagesTest();
}
