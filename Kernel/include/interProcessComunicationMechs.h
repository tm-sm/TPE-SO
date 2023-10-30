#include <stddef.h>

int customPipe(int fd[2]);
size_t readFD(int fd, void *buff, size_t bytes);
size_t writeFD(int fd, const void * buff, size_t bytes);
