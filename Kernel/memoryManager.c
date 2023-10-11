#include "./memoryManager.h"

//INTENTO DE ALOCAR MEMORIA ESTATICA DE MANERA DINAMICA (Para un manipular mas facilmente los strings)
#define MEM_START_ADR 0x0000000000050000

/*
 * Struct -> Define bloques que tienen como dato:
 * El espacio que ocupan.
 * Si esta siendo utilizado.
 * Un puntero al siguiente en la lista.
 */

//Puntero estatico al comienzo del bloque de memoria
static BlockHeader * first_block;

void createMemoryManager() {
    // Inicializo la cabecera del bloque haciendo que apunte al comienzo del array memory
    first_block = (BlockHeader *)MEM_START_ADR;
    first_block->size = MEMORY_SIZE - sizeof(BlockHeader);
    first_block->is_free = 1;
    first_block->next = NULL;
}

void * allocate(size_t size) {
    size_t pageTBU = convertToPageSize(size, PAGE_SIZE);
    // Ahora la idea es buscar un bloque con suficiente espacio
    BlockHeader* curr_block = first_block;
    while (curr_block) {
        if (curr_block->is_free && curr_block->size >= pageTBU) {

            // Si el bloque es mas grande de lo necesario, reservo el espacio y dejo el resto del espacio libre;
            if (curr_block->size >= pageTBU + sizeof(BlockHeader)) {

                BlockHeader* new_block = (BlockHeader*)((char*)curr_block + sizeof(BlockHeader) + pageTBU);    //puntero al nuevo bloque va a estar en la posicion curr_block + el size del struct + lo que quiero reservar de memoria

                //new_block reduce su size en size y sizeof(BlockHeader), esta libre, y su next es el next del current block
                new_block->size = curr_block->size - pageTBU - sizeof(BlockHeader);
                new_block->is_free = 1;
                new_block->next = curr_block->next;


                //current block toma el size pedido, se reserva y el siguiente es el new block
                curr_block->size = pageTBU;
                curr_block->is_free = 0;
                curr_block->next = new_block;

            } else {  //tiene el espacio justo para este bloque => lo reservo
                curr_block->is_free = 0;
            }

            // Retorno un puntero a esta seccion del bloque
            return (void*)((char*)curr_block + sizeof(BlockHeader));
        }
        //si no hay espacio sigo al siguiente
        curr_block = curr_block->next;
    }

    // No hay bloques de memoria disponibles con suficiente espacio
    return NULL;
}

void deallocate(void * ptr) {
    if (ptr == NULL) {
        return;  // Si apunta a nada => no hay nada que liberar
    }

    // Obtengo el puntero al bloque que quiero liberar y cambio el flag is_free a 1
    BlockHeader * block = (BlockHeader *)((char *)ptr - sizeof(BlockHeader));
    block->is_free = 1;

    // Fusion de los bloques consecutivos que esten libres
    BlockHeader * curr_block = first_block;    //recorro desde el principio
    while (curr_block) {
        if (curr_block->is_free) {
            if (curr_block->next && curr_block->next->is_free) {
                curr_block->size += sizeof(BlockHeader) + curr_block->next->size;
                curr_block->next = curr_block->next->next;
            }
        }
        //paso al siguiente
        curr_block = curr_block->next;
    }
}

size_t convertToPageSize(size_t size, size_t pageSize){
    if (pageSize == 0) {
        return 0;
    }

    size_t quotient = size / pageSize;

    return pageSize*quotient + pageSize;
}


/*todo
 * processAllocation handler
 * ProctableHandler
 * ModifyToPageSize
 * AgregarASyscall
 * ConvertirABuddySystem
 * 2880B para TPROC --> 120 Paginas a referenciar de 512B
 */