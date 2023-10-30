#include <stddef.h>
#include <memoryManager.h>
#include <fdManager.h>

#define PIPE_BUFFER_SIZE 1024

struct CustomPipe {
    char buffer[PIPE_BUFFER_SIZE];
    size_t inputFD;
    size_t outputFD;
};

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

size_t readFD(int fd, void* buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(fd);
    if (pipe == NULL) {
        return 0; 
    }
    
    size_t bytesRead = 0;
    while (bytesRead < bytes && pipe->inputFD != pipe->outputFD) {
        ((char*)buff)[bytesRead] = pipe->buffer[pipe->inputFD];
        pipe->inputFD = (pipe->inputFD + 1) % PIPE_BUFFER_SIZE;
        bytesRead++;
    }

    return bytesRead;
}

size_t writeFD(int fd, const void* buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(fd);
    if (pipe == NULL) {
        return 0; 
    }
    
    size_t bytes_written = 0;
    for (size_t i = 0; i < bytes; i++) {
        if ((pipe->outputFD + 1) % PIPE_BUFFER_SIZE != pipe->inputFD) {
            pipe->buffer[pipe->outputFD] = ((const char*)buff)[i];
            pipe->outputFD = (pipe->outputFD + 1) % PIPE_BUFFER_SIZE;
            bytes_written++;
        } else {
            break;
        }
    }

    return bytes_written;
}


