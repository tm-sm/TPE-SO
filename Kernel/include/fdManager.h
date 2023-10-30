
void initializeFileDescriptorManager();

int openFD(void* data);

void* getFDData(int fd);

void closeFD(int fd);

void killFDManager();