# TPE SO - Bootable Operating System in a Docker Image #

This project builds upon the work started in a previous GitHub repository, where we focused on essential components such as system calls, a video and keyboard driver, and various utilities to establish proper Kernel/Userland separation.

In this current phase of the project, we are concentrating on the following key features:

- **Memory Manager:** Implementing a memory management system for efficient allocation and deallocation of memory resources.

- **Multi-Process System:** Developing the infrastructure for managing multiple processes, enabling concurrent execution.

- **Inter-Process Communication:** Establishing mechanisms for processes to communicate and share data.

- **Semaphores:** Implementing semaphores to manage shared resources and synchronize processes.


## Getting Started ##
To run the project, follow these steps:
1. Clone this GitHub repo using:

    ```git clone https://github.com/tm-sm/TPE-SO```
   
2. Create a docker image:

    ```docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -it --name NAME agodio/itba-so:2.0```
3. Compile the code by using:

    ```make -C/root/Toolchain``` and ```make -C/root``` inside the container
4. Run: ```./run.sh``` outside the container
5. On startup a shell console will be displayed, you can see the available instructions by typing ```help```

If using windows, make sure to run these commands from a linux terminal, such as WSL.
 
