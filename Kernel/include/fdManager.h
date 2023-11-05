#include <stddef.h>

void initializeFileDescriptorManager();

int openFD(void* data);

void* getFDData(int fd);

void closeFD(int fd);

int customPipe(int fd[2]);

void closePipe(int pipeFD);

int namedPipe(char * name);

void closeNamedPipe(char * name);

struct NamedPipe getNamedPipe(char * name);

int redirectPipe(int oldfd, int newfd);

size_t read(int pipeFd, char *buff, size_t bytes);

size_t write(int pipeFd, const char * buff, size_t bytes);