#include <stddef.h>

void initializeFileDescriptorManager();

int openFD(void* data);

void* getFDData(int fd);

void closeFD(int fd);

int customPipe(int fd[2]);

void closePipe(int pipeFD[2]);

int redirectPipe(int oldfd, int newfd);

size_t readP(int pipeFd[2], void *buff, size_t bytes);

size_t writeP(int pipeFd[2], const void * buff, size_t bytes);