#include <fdManager.h>
#include <stddef.h>
#include <sems.h>
#include <memoryManager.h>
#include <utils.h>

#define MAX_FILE_DESCRIPTORS 128
#define PIPE_BUFFER_SIZE 128

struct CustomPipe {
    char buffer[PIPE_BUFFER_SIZE];
    size_t readingPos;
    size_t writingPos;
    char readSemaphore[24];
    char writeSemaphore[24];
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

    pipe->readingPos = 0;
    pipe->writingPos = 0;

    uintToBase((uint64_t)pipe,pipe->readSemaphore,16);
    uintToBase((uint64_t)pipe,pipe->writeSemaphore,16);

    strcat(pipe->readSemaphore, " read");
    strcat(pipe->writeSemaphore, " write");

    openSem(pipe->readSemaphore,0);
    openSem(pipe->writeSemaphore, PIPE_BUFFER_SIZE);

    fd[0] = openFD(pipe);
    fd[1] = openFD(pipe);

    return 0;
}

void closePipe(int pipeFD[2]) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(pipeFD[0]);

    closeSem(pipe->readSemaphore);
    closeSem(pipe->writeSemaphore);

    deallocate(pipe);
    closeFD(pipeFD[0]);
    closeFD(pipeFD[1]);
}

size_t readP(int pipeFd[2], void* buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(pipeFd[0]);

    while (1) {
        waitSem(pipe->readSemaphore);

        size_t bytesToRead = 0;

        while (bytesToRead < bytes) {
            if (pipe->readingPos == pipe->writingPos) {

                waitSem(pipe->readSemaphore);
            }

            ((char*)buff)[bytesToRead] = pipe->buffer[pipe->readingPos];
            pipe->readingPos = (pipe->readingPos + 1) % PIPE_BUFFER_SIZE;
            bytesToRead++;

            postSem(pipe->writeSemaphore);
        }

        return bytesToRead;
    }
}

size_t writeP(int pipeFd[2], const void* buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(pipeFd[0]);

    while (1) {
        waitSem(pipe->writeSemaphore);

        size_t bytesWritten = 0;

        while (bytesWritten < bytes) {
            size_t nextWritePos = (pipe->writingPos + 1) % PIPE_BUFFER_SIZE;
            if (nextWritePos == pipe->readingPos) {

                waitSem(pipe->writeSemaphore);
            }

            pipe->buffer[pipe->writingPos] = ((char*)buff)[bytesWritten];
            pipe->writingPos = nextWritePos;
            bytesWritten++;

            postSem(pipe->readSemaphore);
        }

        return bytesWritten;
    }
}
