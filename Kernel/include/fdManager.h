
void initializeFileDescriptorManager();

int openFD(void* data);

void* getFDData(int fd);

void closeFD(int fd);

void killFDManager();

int customDup2(int old_fd, int new_fd);