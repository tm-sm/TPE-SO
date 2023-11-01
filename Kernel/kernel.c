#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <console.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <time.h>
#include <sound.h>
#include <memoryManager.h>
#include <processManager.h>
#include <scheduler.h>
#include <process.h>
#include <fdManager.h>


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

int main()
{
    initializeConsole();
    cPrint("[Kernel Main]");
    cNewline();
    cPrint("Initializing Memory Manager");
    cNewline();
    createMemoryManager();
    cNewline();
    cPrint("Loading IDT descriptors");
    load_idt();
    cNewline();
    cPrint("  Sample data module at 0x");
    cPrintHex((uint64_t)sampleDataModuleAddress);
    cNewline();
    cPrint("  Sample data module contents: ");
    cPrint((char*)sampleDataModuleAddress);
    cPrint("  Sample code module at 0x");
    cPrintHex((uint64_t)sampleCodeModuleAddress);
    cNewline();
    cPrint("[Kernel finished]");
    cNewline();
    cPrint("Initializing Process Manager");
    initializeFileDescriptorManager();
    initializeProcessManager();
    startProcess(NULL, LOW, FOREGROUND, "init", 0, NULL); //pid=1
    cNewline();

    static int pipe_fds[2];//4 5
    static char pipe_buffer[50];
    static char msg[] = "Message from custom pipe";

    /*
    if (customPipe(pipe_fds) == 0) {

        writeFD(pipe_fds[0], msg, 25);
        cPrintDec(pipe_fds[0]);
        cPrintDec(pipe_fds[1]);
        size_t pipe_bytes_read = readFD(pipe_fds[1], pipe_buffer, 24);
        pipe_buffer[pipe_bytes_read] = '\0';
        cPrint(pipe_buffer);
       // printFormat("Read from custom pipe: %s\n", pipe_buffer);

        // Close the custom pipe
        closePipe(pipe_fds);
       // printFormat("Closed custom pipe\n");
    }
*/

    int shellPid = startProcess(sampleCodeModuleAddress, HIGH, FOREGROUND, "shell", 0, NULL);
    while(isProcessAlive(shellPid));
    killProcess(1);


    cPrint("[Exiting System]");
    cNewline();
	return 0;
}
