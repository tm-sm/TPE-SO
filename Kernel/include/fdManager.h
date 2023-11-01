#include <stddef.h>

void initializeFileDescriptorManager();

int openFD(void* data);

void* getFDData(int fd);

void closeFD(int fd);

int customDup2(int old_fd, int new_fd);

int customPipe(int fd[2]);

void closePipe(int pipeFD[2]);

size_t readFD(int fd, void *buff, size_t bytes);

size_t writeFD(int fd, const void * buff, size_t bytes);