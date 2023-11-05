#include <fdManager.h>
#include <stddef.h>
#include <sems.h>
#include <memoryManager.h>
#include <utils.h>
#include <console.h>
#include <keyboardDriver.h>

#define MAX_FILE_DESCRIPTORS 128
#define PIPE_BUFFER_SIZE 128
#define STDOUT 1
#define STDIN 0

struct CustomPipe {
    char buffer[PIPE_BUFFER_SIZE];

    unsigned char fdin;
    unsigned char fdout;

    char rSemaphore[24];
    char wSemaphore[24];
};

struct FileDescriptorEntry {
    unsigned char fd;
    void* data;
    char used;
};

struct FileDescriptorManager {
    struct FileDescriptorEntry entries[MAX_FILE_DESCRIPTORS];
};

static struct FileDescriptorManager* manager;

void initializeFileDescriptorManager() {
    manager = allocate(sizeof(struct FileDescriptorManager));
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        manager->entries[i].used = 0;
    }

    openFD(NULL); //0 STDIN
    openFD(NULL); //1 STDOUT
}

int openFD(void* data) {
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        if (!manager->entries[i].used) {
            manager->entries[i].used = 1;
            manager->entries[i].data = data;
            return manager->entries[i].fd = i;
        }
    }
    return -1;
}

void* getFDData(int fd) {
    if (fd >= 0 && fd < MAX_FILE_DESCRIPTORS && manager->entries[fd].used) {
        return manager->entries[fd].data;
    }
    return NULL;
}

void closeFD(int fd) {
    if (fd >= 0 && fd < MAX_FILE_DESCRIPTORS) {
        manager->entries[fd].used = 0;
        manager->entries[fd].data = NULL;
    }
}

int customPipe(int fd[2]) {
    struct CustomPipe* pipe = (struct CustomPipe*)allocate(sizeof(struct CustomPipe));

    if(pipe == NULL){
        return -1;
    }

    uintToBase((uint64_t)pipe,pipe->wSemaphore,16);

    strcat(pipe->wSemaphore, " wSem");

    openSem(pipe->wSemaphore,1);

    uintToBase((uint64_t)pipe,pipe->rSemaphore,16);

    strcat(pipe->rSemaphore, " rSem");

    openSem(pipe->rSemaphore,0);

    fd[0] = openFD(pipe);
    fd[1] = openFD(pipe);

    pipe->fdin = fd[0];
    pipe->fdout = fd[1];

    return 0;
}

void closePipe(int pipeFD) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(pipeFD);

    closeSem(pipe->rSemaphore);
    closeSem(pipe->wSemaphore);

    deallocate(pipe);
}

int redirectPipe(int oldFd, int newFd){
    if (newFd < 0 || newFd >= MAX_FILE_DESCRIPTORS) {
        return -1;
    }

    void* data = getFDData(oldFd);

    if (data == NULL) {
        return -1;
    }

    if (manager->entries[newFd].used) {
        closeFD(newFd);
    }

    manager->entries[newFd].used = 1;
    manager->entries[newFd].data = data;

    return newFd;
}

size_t read(int fd, char * buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(fd);

    if((!manager->entries[fd].used || pipe->fdout != fd) && fd != STDIN){
        return 0;
    }

    size_t bytesRead = 0;

    switch(fd){
        case STDIN:
            bytesRead = gets(buff, bytes);
            break;
        default:
            waitSem(pipe->rSemaphore);
            for(int i=0; i<bytes; i++) {
                *(buff++) = pipe->buffer[i];
            }
            postSem(pipe->wSemaphore);
            break;
    }

    return bytesRead;
}

size_t write(int fd, const char* buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(fd);

    if((!manager->entries[fd].used || pipe->fdin != fd) && fd != STDOUT){
        return 0;
    }

    size_t bytesWritten = 0;

    switch(fd){
        case STDOUT:
            cPrint((char *) buff);
            bytesWritten = strlen(buff);
            break;
        default:
            waitSem(pipe->wSemaphore);
            for(int i=0; i<bytes; i++) {
                pipe->buffer[i] = *(buff++);
            }
            postSem(pipe->rSemaphore);
            break;
    }

    return bytesWritten;
}
