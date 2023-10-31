include Makefile.inc

KERNEL = kernel.bin
ELF = kernel.elf

ifeq ($(MEMORY_MANAGER),buddy)
    MEMORY_MANAGER_DIR = buddyMemoryManager
else
    MEMORY_MANAGER_DIR = memoryManager
endif

SOURCES_ASM = $(wildcard asm/*.asm)
OBJECTS_ASM = $(SOURCES_ASM:.asm=.o)
SOURCES = $(wildcard *.c) $(MEMORY_MANAGER_DIR)/$(MEMORY_MANAGER_DIR).c
OBJECTS = $(SOURCES:.c=.o)
LOADERSRC = loader.asm
LOADEROBJECT = $(LOADERSRC:.asm=.o)
STATICLIBS =

all: $(KERNEL) $(ELF)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS)  $(OBJECTS_ASM)  $(STATICLIBS)

$(ELF): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T gdb.ld -o $(ELF) $(LOADEROBJECT) $(OBJECTS)  $(OBJECTS_ASM)  $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin memoryManager/*.o buddyMemoryManager/*.o

.PHONY: all buddy clean