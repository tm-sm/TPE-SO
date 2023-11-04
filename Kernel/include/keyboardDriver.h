#ifndef KEYBOARDDRIVER_H
#define KEYBOARDDRIVER_H

#include <stdint.h>

// handles when a key is pressed
void keyboard_handler(uint64_t *registers);

// returns if a key (any) is pressed
uint8_t keyPressed();

// gets char, from the keyboard
int gets(char * s, size_t bytes);

// gets string, from the keyboard
char getc();

// returns 1/0 depending if a key corresponding to a char is being pressed
uint8_t isCharPressed(unsigned char c);

#endif
