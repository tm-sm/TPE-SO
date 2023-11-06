#include <fdManager.h>
#include <stddef.h>
#include <sems.h>
#include <memoryManager.h>
#include <utils.h>
#include <console.h>
#include <keyboardDriver.h>
#include <utils.h>

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

struct NamedPipe{
    char name[24];
    struct CustomPipe pipe;
};

struct NamedPipeList{
    struct NamedPipe * pipe;
    struct NamedPipeList * next;
};


static struct FileDescriptorManager* manager;
static struct NamedPipeList * first;

void initializeFileDescriptorManager() {
    manager = allocate(sizeof(struct FileDescriptorManager));
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        manager->entries[i].used = 0;
    }
    first = (struct NamedPipeList *)allocate(sizeof(struct NamedPipeList));
    first->next = NULL; // Initialize the list as empty

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

    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(fd);
    if(pipe != NULL){
        closePipe(fd);
    }

    if (fd >= 2 && fd < MAX_FILE_DESCRIPTORS) {
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

    if(pipe == NULL){
        return;
    }

    closeSem(pipe->rSemaphore);
    closeSem(pipe->wSemaphore);

    deallocate(pipe);
}

struct NamedPipe *getNamedPipe(const char * name){
    struct NamedPipeList *current = first->next;
    while (current) {
        if (strcmp(current->pipe->name, name) == 0) {
            return current->pipe;
        }
        current = current->next;
    }
    return NULL;
}

int setToNamedPipeFd(int *proc1, int *proc2, const char * name){
    struct NamedPipe * pip = getNamedPipe(name);
    if(pip == NULL){
        return -1;
    }

    *proc1 = pip->pipe.fdin;
    *proc2= pip->pipe.fdout;
    return 0;
}

int namedPipe(const char * name){
    struct NamedPipeList *newPipeEntry = (struct NamedPipeList *)allocate(sizeof(struct NamedPipeList));

    int fd[2];

    if (!newPipeEntry) {
        // Allocation failed, handle the error
        return -1;
    }

    if (customPipe(fd) != 0) {
        deallocate(newPipeEntry);
        closePipe(fd[0]);
        return -1;
    }


    strcpy(newPipeEntry->pipe->name, name);

    newPipeEntry->next = first->next;

    first->next = newPipeEntry;

    return 0;
}

void closeNamedPipe(const char * name){
    struct NamedPipeList *current = first;
    struct NamedPipeList *previous = NULL;

    while (current) {
        if (strcmp(current->pipe->name, name) == 0) {
            closePipe(current->pipe->pipe.fdin);
            if (previous) {
                previous->next = current->next;
            } else {
                first->next = current->next;
            }
            deallocate(current->pipe);
            deallocate(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

size_t read(int fd, char * buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(fd);

    if((fd != STDIN) && (pipe == NULL || !manager->entries[fd].used || pipe->fdout != fd)){
        return 0;
    }

    size_t bytesRead;
    int i=0;

    switch(fd){
        case STDIN:
            bytesRead = gets(buff, bytes);
            break;
        default:
            waitSem(pipe->rSemaphore);
            for(; i < bytes && i < PIPE_BUFFER_SIZE - 1 && pipe->buffer[i] != '\0'; i++) {
                *(buff++) = pipe->buffer[i];
            }
            *buff = '\0';
            bytesRead = i;
            postSem(pipe->wSemaphore);
            break;
    }

    return bytesRead;
}

size_t write(int fd, const char* buff, size_t bytes) {
    struct CustomPipe* pipe = (struct CustomPipe*)getFDData(fd);

    if((fd != STDOUT) && (pipe == NULL || !manager->entries[fd].used || pipe->fdin != fd)){
        return 0;
    }

    size_t bytesWritten;
    int i = 0;

    if(bytes >= 1 && buff[0] == EOF) {
        postSem(pipe->wSemaphore);
    }

    switch(fd){
        case STDOUT:
            cPrint((char *) buff);
            bytesWritten = strlen(buff);
            break;
        default:
            waitSem(pipe->wSemaphore);
            for(; i < bytes && i < PIPE_BUFFER_SIZE - 1; i++) {
                pipe->buffer[i] = *(buff++);
            }
            pipe->buffer[i] = '\0';
            bytesWritten = i;
            postSem(pipe->rSemaphore);
            break;
    }

    return bytesWritten;
}
