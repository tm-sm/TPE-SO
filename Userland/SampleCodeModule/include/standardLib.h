#ifndef TPE_ARQUI_STANDARDLIB_H
#define TPE_ARQUI_STANDARDLIB_H

#include <stdint.h>
#include <stddef.h>

/**
 * Calls a system interruption
 * @param rdi
 * @param rsi
 * @param rdx
 * @param rcx
 * @param r8
 * @param r9
 */
uint64_t interrupt(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

uint64_t join(uint32_t upper, uint32_t lower);

/**
 * Writes a character in the console
 * @param c
 */
void putChar(char c);

/**
 * Retrieves a key press from the keyboard, waits until input is recieved before continuing
 * @return Pressed character
 */
char getChar();

size_t getStrn(char* buff, size_t len);

/**
 * Prints a string to screen, can print parameters passed
 * @param format %d for decimal | %x for hexadecimal | %s for string
 * @param ... Values representing the value requested in order of appearance
 */
void printFormat(const char*format,...);

/**
 * Compares two strings
 * @param s1
 * @param s2
 * @return 0 if the strings are equal
 */
int compString(const char *s1,const char *s2);

char * strtok(char * str, const char * delim);
char * strcat(char* dest, const char* src);
char * strcpy(char* dest, const char* src);
size_t strlen(const char* str);
void putStrn(char* s);
int strncmp(const char* str1, const char* str2, size_t n);
int strcmp(const char* str1, const char* str2);

int atoi(const char* str);

uint8_t isCharPressed(char c);
#endif //TPE_ARQUI_STANDARDLIB_H
