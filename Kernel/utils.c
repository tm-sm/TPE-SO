#include <stdint.h>
#include <stddef.h>

uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base) {
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    //Calculate characters for each digit
    do {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);

    // Terminate string in buffer.
    *p = 0;

    //Reverse string in buffer.
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

int strcmp(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] && (str1[i] == str2[i])) i++;

    return (int)(str1[i] - str2[i]);
}

char * strcpy(char* dest, const char* src) {
    int i = 0;
    while ((dest[i] = src[i])) i++;

    return dest;
}


size_t strlen(const char* str) {
    size_t len = 0;
    while(str[len]) len++;
    return len;
}

char * strcat(char* dest, const char* src) {
    size_t dest_len = strlen(dest);
    int i = 0;
    while(src[i] != '\0') {
        dest[dest_len++] = src[i++];
    }
    dest[dest_len] = '\0';
    return dest;
}
