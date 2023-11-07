#include <stddef.h>

/**
 * Initializes the file descriptor Manager Entry Table and the 5 FIFO's that could be used
 */
void initializeFileDescriptorManager();
/**
 * Asigns a file descriptor from the Manager Entry Table and associates a pointer to the data ( it could be a Process, Pipe, etc..)
 * returns file descriptor
 */
int openFD(void* data);
/**
 * If the fd is valid it will retrieve the data associated to it
 */
void* getFDData(int fd);

/**
 *  Will return the fd of the oposite end of the pipe if one is associated to the fd.
 *  returns -1 if no pipe or fd is found
 */
int getOtherEndOfPipe(int fd);
/**
 * unasigns the given fd except for the reserved fd  (from 0 to 11)
 */
void closeFD(int fd);
/**
 * Opens a pipe between the file descriptors.
 * Returns -1 if the pipe could not be created.
 * 0 on success
 */
int customPipe(int fd[2]);
/**
 * Closes the pipe that contains the corresponding file descriptor
 */
void closePipe(int pipeFD);

/**
 * Allows a FIFO to be used. It will connect the stdout end of proc 1 to stdin of the pipe. And stdin of proc 2 to the stdout of the pipe.
 * returns -1 on failure
 * 0 on success
 */
int setToNamedPipeFd(int *proc1, int *proc2, const char * name);
/**
 * Shows a list of the current FIFOs
 */
void displayFIFO();
/**
 * This function will read n bytes from a file Descriptor to a buffer. If its 0 --> It will consider it stdin (keyboard), 2 > It will be a pipe
 * returns number of bytes read.
 */
size_t read(int pipeFd, char *buff, size_t bytes);
/**
 * This function will write n bytes from a buffer to a file descriptor. If its 1 --> It will consider it stdout (screen), 2 > It will be a pipe
 * returns number of bytes written.
 */
size_t write(int pipeFd, const char * buff, size_t bytes);