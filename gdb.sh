gdb
target remote host.docker.internal:1234
add-symbol-file Kernel/kernel.elf 0x100000
add-symbol-file Userland/sampleCodeModule.elf 0x400000