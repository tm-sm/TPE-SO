
all:  bootloader kernel userland image
buddy: bootloader kernelBuddy userland imageBuddy

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

kernelBuddy:
	cd Kernel; make all MEMORY_MANAGER=buddy

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all


imageBuddy: kernelBuddy bootloader userland
	cd Image; make all

clean:
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: bootloader image collections kernel userland all clean
