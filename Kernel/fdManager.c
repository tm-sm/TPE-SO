#include <fdManager.h>
#include <stddef.h>
#include <memoryManager.h>
#include <console.h>

#define MAX_FILE_DESCRIPTORS 128

#define PIPE_BUFFER_SIZE 512

// // TODO AGREGAR SEMAFOROS A LOS PIPES // //

struct CustomPipe {
    char buffer[PIPE_BUFFER_SIZE];
    unsigned char inputFD;
    unsigned char outputFD;
};

struct FileDescriptorEntry {
    unsigned char fd;
    void* data;
    char used;
};

struct FileDescriptorManager {
    struct FileDescriptorEntry entries[MAX_FILE_DESCRIPTORS];
};

static struct FileDescriptorManager * manager;

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

int customDup2(int oldFD, int newFD) {
    if (oldFD < 0 || oldFD >= MAX_FILE_DESCRIPTORS || newFD < 0 || newFD >= MAX_FILE_DESCRIPTORS || oldFD == newFD) {
        return -1;
    }

    struct FileDescriptorEntry* oldEntry = &manager->entries[oldFD];

    if (!oldEntry->used) {
        return -1;
    }

    if (manager->entries[newFD].used) {
        closeFD(newFD);
    }


    manager->entries[newFD] = *oldEntry;
    return newFD;
}

int customPipe(int fd[2]) {
    struct CustomPipe* customPipe = (struct CustomPipe*)allocate(sizeof(struct CustomPipe));
    if (customPipe == NULL) {
        return -1;
    }

    int fdRead = openFD(customPipe);
    int fdWrite = openFD(customPipe);

    fd[0] = fdRead;
    fd[1] = fdWrite;

    return 0;
}

void closePipe(int pipeFD[2]) {
    void*addr = getFDData(pipeFD[0]);
    
    if (pipeFD[0] >= 0) {
        closeFD(pipeFD[0]); 
        pipeFD[0] = -1;     
    }

    if (pipeFD[1] >= 0) {
        closeFD(pipeFD[1]); 
        pipeFD[1] = -1;     
    }
    
    deallocate(addr);
}

size_t readFD(int fd, void* buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(fd);
    if (pipe == NULL) {
        return 0;
    }

    size_t bytesRead = 0;
    while (bytesRead < bytes) {
        ((char*)buff)[bytesRead] = pipe->buffer[bytesRead];
        bytesRead++;
    }

    return bytesRead;
}

size_t writeFD(int fd, const void* buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(fd);
    if (pipe == NULL) {
        return 0;
    }

    size_t bytesWritten = 0;
    for (size_t i = 0; i < bytes; i++) {
            pipe->buffer[bytesWritten] = ((const char*)buff)[i];
            bytesWritten++;
    }

    return bytesWritten;
}

