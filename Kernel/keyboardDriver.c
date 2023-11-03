#include <stdint.h>
#include <keyboardDriver.h>
#include <lib.h>
#include <processManager.h>
#include <time.h>

extern unsigned char keydown();

#define NONE -1

#define LSHIFT 0x2A
#define RSHIFT 0x36
#define LCTRL 0x1D
#define LALT 0x38
#define CAPS_LOCK 0x3A
#define C 0x2E

#define F1 0x3B

#define NO_INPUT 0x00

#define TRUE 1
#define FALSE 0

static uint8_t shift = FALSE;
static uint8_t ctrl = FALSE;
static uint8_t alt = FALSE;
static uint8_t caps_lock = FALSE;
static uint8_t ctrlOperation = FALSE;

static char keyBuffer = NO_INPUT;
static int blockedProcess = NONE;

const unsigned char kbdusNoShift[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd',
    'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const unsigned char kbdusWithShift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 'D',
    'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '|', 0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t keyMap[256] = {FALSE};

uint8_t keyPressed() {
    return keyBuffer;
}

uint8_t isCharPressed(unsigned char c) {
    if(isCurrentProcessInForeground()) {
        return keyMap[c];
    }
    return FALSE;
}

static char readBuffer() {
    if(isCurrentProcessInForeground()) {
        char ret = keyBuffer;
        keyBuffer = NO_INPUT;
        return ret;
    }
    return NO_INPUT;
}

static void addToBuffer(uint8_t keycode) {
    if (keycode == NO_INPUT) {
        keyBuffer = NO_INPUT;
    }
    unsigned char c = (kbdusNoShift[keycode]);
    uint8_t isLetter = (c >= 'a' && c <= 'z');
    uint8_t auxShift = (caps_lock && isLetter)?(!shift):(shift);
    if(auxShift) {
        keyBuffer = kbdusWithShift[keycode];
    } else {
        keyBuffer = c;
    }
    if(blockedProcess != NONE) {
        int pid = blockedProcess;
        blockedProcess = NONE;
        unblockProcess(pid);
    }
}

static void setKeyMap(uint8_t keycode, uint8_t condition) {
    if (keycode == NO_INPUT) {
        return;
    }
    unsigned char c = (kbdusNoShift[keycode]);
    uint8_t isLetter = (c >= 'a' && c <= 'z');
    uint8_t auxShift = (caps_lock && isLetter)?(!shift):(shift);
    if(auxShift) {
        keyMap[kbdusWithShift[keycode]] = condition;
        return;
    }
    keyMap[c] = condition;
}

void keyboard_handler(uint64_t* registers) {
    unsigned char code = keydown();
    if(code == NO_INPUT) {
        addToBuffer(NO_INPUT);
    }
    uint8_t keyRelease = FALSE;
    unsigned char keycode = code & 0x7F; // no diferencia entre release o no

    if(code & 0x80) {
        keyRelease = TRUE;
    }
    if(ctrl && !ctrlOperation) {
        ctrlOperation = TRUE;
        switch(keycode) {
            case C:
                killProcessInForeground();
                return;
            default:
                //continues
        }
    }else if(!ctrl){
        ctrlOperation = FALSE;
    }

    if(keycode == LCTRL) {
        ctrl = (keyRelease)?(FALSE):(TRUE);
    }
    else if(keycode == LALT) {
        alt = (keyRelease)?(FALSE):(TRUE);
    }
    else if(keycode == LSHIFT || keycode == RSHIFT) {
        shift = (keyRelease)?(FALSE):(TRUE);
    }
    else if(keycode == CAPS_LOCK) {
        caps_lock = (keyRelease)?(caps_lock):(!caps_lock);
    }
    else if(keycode == F1) {
        displayRegs(registers);
    }
    else if(!keyRelease) {
        addToBuffer(keycode);
        setKeyMap(keycode, TRUE);
    }
    else if(keyRelease) {
        setKeyMap(keycode, FALSE);
    }
}

char getc(){
    if(isCurrentProcessInForeground()) {
        uint8_t c = readBuffer();
        while (c == NO_INPUT && blockedProcess == NONE) {
            blockedProcess = getActiveProcessPid();
            blockCurrentProcess();
            c = readBuffer();
        }
        return (char) c;
    }
    return NO_INPUT;
}

void gets(char * s) {
    int i = 0;
    char c;
    while((c=keyBuffer)!='\n') {
        s[i] = c;
        i++;
    }
    s[i]='\0';
}
