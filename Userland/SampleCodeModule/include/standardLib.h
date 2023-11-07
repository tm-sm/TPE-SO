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

static char * itoa( uint64_t value, char * str, int base );

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

/**
 * Split a string into tokens using the specified delimiters.
 *
 * @param str   The string to split (modify this string)
 * @param delim The string containing delimiters
 * @return      A pointer to the next token in 'str' or NULL if no more tokens are found
 */
char *strtok(char *str, const char *delim);

/**
 * Concatenate (append) the source string 'src' to the destination string 'dest'.
 *
 * @param dest The destination string to which 'src' will be appended
 * @param src  The source string that will be appended to 'dest'
 * @return     A pointer to the resulting 'dest' string
 */
char *strcat(char *dest, const char *src);

/**
 * Copy the source string 'src' to the destination string 'dest'.
 *
 * @param dest The destination string where 'src' will be copied
 * @param src  The source string that will be copied to 'dest'
 * @return     A pointer to the resulting 'dest' string
 */
char *strcpy(char *dest, const char *src);

/**
 * Calculate the length (number of characters) of a string.
 *
 * @param str The string for which to calculate the length
 * @return    The length of the string
 */
size_t strlen(const char *str);

/**
 * Print a string to the standard output (stdout) without a newline.
 *
 * @param s The string to print
 */
void putStrn(char *s);

/**
 * Compare the first 'n' characters of two strings, 'str1' and 'str2'.
 *
 * @param str1 The first string to compare
 * @param str2 The second string to compare
 * @param n    The number of characters to compare
 * @return     0 if the 'n' characters of 'str1' and 'str2' are equal, a positive value if 'str1' is greater,
 *            or a negative value if 'str2' is greater
 */
int strncmp(const char *str1, const char *str2, size_t n);

/**
 * Compare two strings, 'str1' and 'str2'.
 *
 * @param str1 The first string to compare
 * @param str2 The second string to compare
 * @return     0 if 'str1' and 'str2' are equal, a positive value if 'str1' is greater, or a negative value if 'str2' is greater
 */
int strcmp(const char *str1, const char *str2);

/**
 * Convert a string to an integer.
 *
 * @param str The string to convert to an integer
 * @return    The integer value represented by the string
 */
int atoi(const char *str);

uint8_t isCharPressed(char c);
#endif //TPE_ARQUI_STANDARDLIB_H
