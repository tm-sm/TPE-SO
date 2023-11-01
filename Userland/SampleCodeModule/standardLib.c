#include <standardLib.h>
#include <stdarg.h>
#include <system.h>
#define SYSWRITE 0
#define SYSREAD 1

int getDigits(int n);
void _printDec(int value, int len, uint8_t padding0, char* buff);
void _printHex(uint64_t value, char* buff);

static char * itoa( uint64_t value, char * str, int base )
// code taken from https://wiki.osdev.org/Printing_To_Screen
{
    char * rc;
    char * ptr;
    char * low;
    if ( base < 2 || base > 36 ) {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    if ( value < 0 && base == 10 ) {
        *ptr++ = '-';
    }
    low = ptr;
    do {
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    *ptr-- = '\0';
    while( low < ptr ) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}



int atoi(const char* str) {
    int result = 0;
    int sign = 1;

    while (*str == ' ' || (*str >= 9 && *str <= 13)) {
        str++;
    }

    if (*str == '-' || *str == '+') {
        sign = (*str == '-') ? -1 : 1;
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}

void putChar(char c){ //a partir del segundo parametro no importa lo que le ponga
    interrupt(SYSWRITE,(uint64_t) &c,1,0,0,0);
}

void putStrn(char* s) {
    int i;
    for(i=0;s[i]!='\0';i++){
    }
    interrupt(SYSWRITE, (uint64_t)s, i, 0, 0, 0);
}

char getChar(){
    char c;
    //this prevents system calls from being blocked by the sys_read interruption
    while(1) {
        if(keyPress()) {
            interrupt(SYSREAD,(uint64_t)&c,0,0,0,0);
            return c;
        }
    }
}

uint8_t isCharPressed(char c) {
    uint8_t v;
    interrupt(9, (uint64_t)&v, c, 0, 0, 0);
    return v;
}

void printFormat(const char* format, ...) {
    char buff[17];
    va_list args;
    va_start(args, format);

    while (*format != '\0') {
        if (*format == '%') {
            format++; // Move past '%'
            int len = 0;
            uint8_t padding0 = 0;
            if(*format == '0') {
                padding0 = 1;
                format++;
            }
            if(*format >= '1' && *format <= '9') {
                len = *format - '0';
                format++;
            }
            switch (*format) {
                case 'd': {
                    int value = va_arg(args, int);
                    _printDec(value, len, padding0, buff);
                    break;
                }
                case 's': {
                    char* value = va_arg(args, char*);
                    putStrn(value);
                    break;
                }
                case 'x':{
                    format++;
                    uint64_t value =va_arg(args,uint64_t);
                    _printHex(value, buff);
                    break;
                }
                default: {
                    putChar(*format);
                    break;
                }
            }
        } else {
            putChar(*format);
        }
        format++; // Move to the next character
    }
    va_end(args);
}

int _pow(int base, int power) {
    //this can't handle a negative power value
    if(power <= 0) {
        return 1;
    }
    int result = base;
    for(int i=0; i< power - 1; i++) {
        result = result * base;
    }
    return result;
}

void _printDec(int value, int len, uint8_t padding0, char* buff) {
    char tempBuff[17];
    int i = 0;
    int isNegative = 0;
    if (value < 0) {
        isNegative = 1;
        value = -value;
    }
    while (value > 0) {
        tempBuff[i++] = (char)((value % 10) + '0');
        value /= 10;
    }
    while (i < len) {
        if (padding0) {
            tempBuff[i++] = '0';
        } else {
            tempBuff[i++] = ' ';
        }
    }
    if(isNegative) {
        tempBuff[i++] = '-';
    }
    for(int j = i - 1; j >= 0; j--) {
        putChar(tempBuff[j]);
    }
}

void _printHex(uint64_t value, char* buff) {
    putStrn("0x");
    putStrn(itoa(value,buff,16));
}

int compString(const char*s1,const char*s2){
    while (*s1 == *s2++)
		if (*s1++ == 0)
			return 0;
	return (*s1 - *--s2);
}

int getDigits(int n){
    int digits=0;
    while(n!=0){
        n=n/10;
        digits++;
    }
    return digits;
}

int strcmp(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] && (str1[i] == str2[i])) i++;

    return (int)(str1[i] - str2[i]);
}

int strncmp(const char* str1, const char* str2, size_t n) {
    int i = 0;
    while(n-- && str1[i] && str2[i] && str1[i] == str2[i]) i++;
    return (int)(str1[i] - str2[i]);
}

size_t strlen(const char* str) {
    size_t len = 0;
    while(str[len]) len++;
    return len;
}

char * strcpy(char* dest, const char* src) {
    int i = 0;
    while ((dest[i] = src[i])) i++;

    return dest;
}

char * strcat(char* dest, const char* src) {
    size_t dest_len = strlen(dest);
    int i = 0;
    while(src[i] != '\0'){
        dest[dest_len++] = src[i++];
    }
    dest[dest_len] = '\0';
    return dest;
}

char* strtok(char* str, const char* delim) {
    static char* last_ptr = NULL;

    if (str != NULL) {
        last_ptr = str;
    } else if (last_ptr == NULL || *last_ptr == '\0') {
        return NULL;
    }

    char* token = last_ptr;
    while (*last_ptr != '\0') {
        int i;
        for (i = 0; delim[i] != '\0'; i++) {
            if (*last_ptr == delim[i]) {
                *last_ptr = '\0';
                last_ptr++;
                if (token != last_ptr) {
                    return token;
                } else {
                    token = last_ptr;
                    continue;
                }
            }
        }
        last_ptr++;
    }
    last_ptr = NULL;
    return token;
}
