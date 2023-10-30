#include <fdManager.h>
#include <stddef.h>
#include <memoryManager.h>

#define MAX_FILE_DESCRIPTORS 64

struct FileDescriptorEntry {
    int fd;
    void* data;
    char in_use;
};

struct FileDescriptorManager {
    struct FileDescriptorEntry entries[MAX_FILE_DESCRIPTORS];
};

static struct FileDescriptorManager * manager;

void initializeFileDescriptorManager() {
    manager = allocate(sizeof(struct FileDescriptorManager));
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        manager->entries[i].in_use = 0;
    }
}

int openFD(void* data) {
    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        if (!manager->entries[i].in_use) {
            manager->entries[i].in_use = 1;
            manager->entries[i].data = data;
            return manager->entries[i].fd = i;
        }
    }
    return -1;
}

void* getFDData(int fd) {
    if (fd >= 0 && fd < MAX_FILE_DESCRIPTORS && manager->entries[fd].in_use) {
        return manager->entries[fd].data;
    }
    return NULL;
}

void closeFD(int fd) {
    if (fd >= 0 && fd < MAX_FILE_DESCRIPTORS) {
        manager->entries[fd].in_use = 0;
        manager->entries[fd].data = NULL;
    }
}

void killFDManager(){
    int fd = 0;
    while(fd < MAX_FILE_DESCRIPTORS){
        fd++;
        closeFD(fd);
    }
    deallocate(manager);
}

int customDup2(int oldFD, int newFD) {
    if (oldFD < 0 || oldFD >= MAX_FILE_DESCRIPTORS || newFD < 0 || newFD >= MAX_FILE_DESCRIPTORS || oldFD == newFD) {
        return -1;
    }

    struct FileDescriptorEntry* oldEntry = &manager->entries[oldFD];
    
    if (!oldEntry->in_use) {
        return -1; 
    }
    
    if (manager->entries[newFD].in_use) {
        if (oldEntry->data != NULL) {
            oldEntry->data = NULL;
        }
        closeFD(newFD);
    }

    // Duplicate the oldFD to newFD
    manager->entries[newFD] = * oldEntry;
    return newFD;
}