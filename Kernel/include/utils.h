#ifndef TPE_ARQUI_UTILS_H
#define TPE_ARQUI_UTILS_H
#include <stdint.h>
#include <stddef.h>

#define EOF (-1)

// converts uint to specified base
uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);
int strcmp(const char* str1, const char* str2);
char * strcpy(char* dest, const char* src);
char * strcat(char* dest, const char* src);
size_t strlen(const char * str);
#endif //TPE_ARQUI_UTILS_H
