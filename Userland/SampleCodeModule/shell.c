#include <system.h>
#include <commands.h>
#include <graphics.h>
#include <standardLib.h>

#define CONSOLE_X_DIM 96

typedef char cLine[CONSOLE_X_DIM];
static cLine prompt;
static int promptDim = 0;

void shellLoop();

void clearPrompt() {
    for(int i=0; i<CONSOLE_X_DIM; i++) {
        prompt[i] = '\0';
    }
    promptDim = 0;
}

void writePromptIcon() {
    printFormat("$~");
}

void writeCursor() {
    putChar('_');
}

void eraseCursor() {
    putChar('\b');
}

void startShell() {
    putChar('\n');
    shellLoop();
}

void shellLoop() {
    //waits for input and stores it in prompt
    int pid;
    char c = 0;
    writePromptIcon();
    writeCursor();
    while((c = getChar()) != 27 ) {// 'esc'

        if(c == '\n') {
            //executes the command
            eraseCursor();
            putChar(c);
            if((pid = parseCommand(prompt)) != -1) {
                if(isProcessInForeground(pid)) {
                    while(isProcessAlive(pid));
                }
            }
            clearPrompt();
            putChar('\n');
            writePromptIcon();
            writeCursor();
        }

        else if(c == '\b') {
            eraseCursor();
            if(promptDim > 0) {
                putChar(c);
                prompt[--promptDim] = '\0';
            }
            writeCursor();
        }

        else if(promptDim < CONSOLE_X_DIM - 3){
            eraseCursor();
            putChar(c);
            prompt[promptDim++] = c;
            writeCursor();
        }
    }
    clearScreen();
}
