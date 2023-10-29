#include <stdint.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <console.h>
#include <time.h>
#include <sound.h>
#include <scheduler.h>
#include <processManager.h>
#define BASE_PARAMS uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9
#define COMPLETE_PARAMS uint64_t rdi, BASE_PARAMS

typedef uint64_t (*functionPtr)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

uint64_t sys_write(BASE_PARAMS); // code 0
uint64_t sys_read(BASE_PARAMS); // code 1
uint64_t sys_draw(BASE_PARAMS); // code 2
uint64_t sys_double_buffer(BASE_PARAMS); // code 3
uint64_t sys_get_time(BASE_PARAMS); // code 4
uint64_t sys_detect_key_press(BASE_PARAMS); // code 5
uint64_t sys_wait(BASE_PARAMS); // code 6
uint64_t sys_sound(BASE_PARAMS); // code 7
uint64_t sys_nop(BASE_PARAMS); // code 8
uint64_t sys_is_char_pressed(BASE_PARAMS); // code 9
uint64_t sys_create_process(BASE_PARAMS); // code 10
uint64_t sys_kill_process(BASE_PARAMS); // code 11
uint64_t sys_set_process_foreground(BASE_PARAMS); // code 12
uint64_t sys_get_own_pid(BASE_PARAMS); // code 13
uint64_t sys_is_process_alive(BASE_PARAMS); // code 14
uint64_t sys_memory_manager(BASE_PARAMS); // code 15
uint64_t sys_check_process_foreground(BASE_PARAMS); // code 16
extern uint64_t* current_regs();

extern void _sti();

//TODO cambiar las funciones que usaban punteros para retornar a que usen rax

functionPtr interruptions[] = {sys_write, sys_read, sys_draw, sys_double_buffer,
                               sys_get_time, sys_detect_key_press,
                               sys_wait, sys_sound, sys_nop, sys_is_char_pressed,
                               sys_create_process, sys_kill_process, sys_set_process_foreground,
                               sys_get_own_pid, sys_is_process_alive, sys_memory_manager,
                               sys_check_process_foreground};

uint64_t swInterruptDispatcher(COMPLETE_PARAMS) {
    if(rdi >= sizeof(interruptions)) {
        return -1;
    }
    uint64_t ret = interruptions[rdi](rsi, rdx, rcx, r8, r9);
    return ret;
}

//ID= 0
//rsi= char* pointing to the start of the string
//rdx= amount of chars that should be written
// returns= nothing
uint64_t sys_write(BASE_PARAMS) {
  char* s=(char*)rsi;
  for(int i=0;i<rdx ;i++){
    if(s[i]=='\0')
        return 0;
    cPrintChar(s[i]);
  }
  return 0;
}

//ID= 1
//rsi= char* pointing to the tar
// returns= nothing TODO
uint64_t sys_read(BASE_PARAMS) {
    *(char*)rsi=getc();
    return 0;
}

//ID= 2
//rsi= 0 -> pixel || 1 -> line || 2 -> empty rectangle || 3 -> rectangle || 4 -> empty circle || 5 -> filled circle || 6 -> clear screen
//rdx= INITIAL COORDINATES :: upper half -> x0 || lower half -> y0
//rcx= FINAL COORDINATES (used for lines :: upper half -> x1 || lower half -> y1
//r8= DIMENSIONS (used by circles and rectangles) :: upper half -> rectangle height || lower half -> rectangle width // circle radius
//r9= COLOR :: in hex values
// returns= 0 on success, -1 on fail
uint64_t sys_draw(BASE_PARAMS) {
    uint32_t x0 = (uint32_t) (rdx >> 32);
    uint32_t y0 = (uint32_t) rdx;
    uint32_t x1 = (uint32_t) (rcx >> 32);
    uint32_t y1 = (uint32_t) rcx;

    uint32_t height = (uint32_t) (r8 >> 32);
    uint32_t width = (uint32_t) r8;

    uint32_t hexColor = (uint32_t) r9;
    switch(rsi) {
        case 0:
            putHexPixel(hexColor, x0, y0);
            break;
        case 1:
            drawHexLine(hexColor, x0, y0, x1, y1);
            break;
        case 2:
            drawEmptyHexRectangle(hexColor, x0, y0, width, height);
            break;
        case 3:
            drawHexRectangle(hexColor, x0, y0, width, height);
            break;
        case 4:
            drawEmptyHexCircle(hexColor, x0, y0, width);
            break;
        case 5:
            drawHexCircle(hexColor, x0, y0, width);
            break;
        case 6:
            clearScreen();
            break;
        default:
            return -1;
    }
    return 0;
}

//ID= 3
//rsi= INSTRUCTION :: 0 -> disables double buffering || 1 -> enables double buffering || 2 -> swaps buffers
// returns= nothing
uint64_t sys_double_buffer(BASE_PARAMS) {
    switch(rsi) {
        case 0:
            disableDoubleBuffering();
            break;
        case 1:
            enableDoubleBuffering();
            break;
        case 2:
            drawBuffer();
            break;
        default:
            return -1;
    }
    return 0;
}

//ID=4
//rsi= DATA TYPE :: 0 -> seconds || 1 -> minutes || 2 -> hours || 3 -> day || 4 -> month || 5 -> year
//rdx= pointer to an unsigned int, the value is stored in this position
// returns= nothing TODO
uint64_t sys_get_time(BASE_PARAMS) {
    switch(rsi) {
        case 0:
            *(unsigned int*)rdx = seconds();
            break;
        case 1:
            *(unsigned int*)rdx = minutes();
            break;
        case 2:
            *(unsigned int*)rdx = hours();
            break;
        case 3:
            *(unsigned int*)rdx = day();
            break;
        case 4:
            *(unsigned int*)rdx = month();
            break;
        case 5:
            *(unsigned int*)rdx = year();
            break;
        default:
            return -1;
    }
    return 0;
}


//ID=5
// returns= 0 if key buffer is empty, 1 if not
uint64_t sys_detect_key_press(BASE_PARAMS) {
    return keyPressed();
}

//ID=6
//rsi= milliseconds to wait in unsigned long
// returns= nothing
uint64_t sys_wait(BASE_PARAMS) {
    wait(rsi);
    return 0;
}

// ID= 7
// rsi= frequency of beep
// rdx= duration of beep
// returns= nothing
uint64_t sys_sound(BASE_PARAMS) {
    //beep needs the timer tick interruption to work, this isn't pretty, but it won't work without it
    play_beep(rsi, rdx);
    return 0;
}

// ID= 8
// returns= nothing
uint64_t sys_nop(BASE_PARAMS){
    //does nothing
    return 0;
}

// ID= 9
// rsi= char corresponding to a key
// rdx= pointer to an uint8_t, returns 1 if the key corresponding to the char is being pressed, 0 if not
// returns= nothing TODO
uint64_t sys_is_char_pressed(BASE_PARAMS) {
    *(uint8_t*) rsi = isCharPressed(rdx);
    return 0;
}

// ID= 10
// rsi= instruction pointer
// rdx= PRIORITY :: 0 -> HIGH || 1 -> MEDIUM || 2 -> LOW
// rcx= DRAWING STATE :: define FOREGROUND (1) / define BACKGROUND (0)
// r8= char* argv[]
// returns= pid on success, -1 on fail
uint64_t sys_create_process(BASE_PARAMS) {
    return startProcess((void*)rsi, (int)rdx, rcx, (char*)r8, 0, (char**)r9);
}

// ID= 11
// rsi= pid
// returns= nothing
uint64_t sys_kill_process(BASE_PARAMS) {
    killProcess((int)rsi);
    return 0;
}

// ID= 12
// rsi= pid
// rdx= 0 -> background || 1 -> foreground
// returns= nothing
uint64_t sys_set_process_foreground(BASE_PARAMS) {
    setProcessForeground((int)rsi, (int)rdx);
    return 0;
}

// ID= 13
// returns= pid
uint64_t sys_get_own_pid(BASE_PARAMS) {
    return getActiveProcessPid();
}

// ID= 14
// returns= 1 -> alive || 0 -> dead
uint64_t sys_is_process_alive(BASE_PARAMS) {
    return isProcessAlive(rsi);
}

// ID= 15
// rsi= 0 -> allocate || 1 -> reallocate || 2 -> deallocate
// rdx= memory address for reallocate and deallocate
// rcx= size for allocate and reallocate
// returns= memory address for allocate and reallocate, 0 for deallocate
uint64_t sys_memory_manager(BASE_PARAMS) {
    switch(rsi) {
        case 0:
            return (uint64_t) allocate(rcx);
        case 1:
            return (uint64_t) reallocate((void*)rdx, rcx);
        case 2:
            deallocate((void*)rdx);
        default:
            return 0;
    }
}

// ID= 16
// rsi= pid
// returns= 1 if in foreground, 0 if not
uint64_t sys_check_process_foreground(BASE_PARAMS) {
    return isProcessInForeground((int)rsi);
}
