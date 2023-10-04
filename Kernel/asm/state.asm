
%macro saveState 1
	mov rax, %1
	mov rbx, %1 + 8
	mov rcx, %1 + 16
	mov rdx, %1 + 24
	mov rbp, %1 + 32
	mov rdi, %1 + 40
	mov rsi, %1 + 48
	mov r8, %1 + 56
	mov r9, %1 + 64
	mov r10, %1 + 72
	mov r11, %1 + 80
	mov r12, %1 + 88
	mov r13, %1 + 96
	mov r14, %1 + 104
	mov r15, %1 + 112
	mov rflags, %1 + 120
	mov rsp, %1 + 128
	mov rip, %1 + 136
%endmacro

%macro loadState 0
	mov %1, rax
    mov [%1 + 8], rbx
    mov [%1 + 16], rcx
    mov [%1 + 24], rdx
    mov [%1 + 32], rbp
    mov [%1 + 40], rsp
    mov [%1 + 48], rdi
    mov [%1 + 56], rsi
    mov [%1 + 64], r8
    mov [%1 + 72], r9
    mov [%1 + 80], r10
    mov [%1 + 88], r11
    mov [%1 + 96], r12
    mov [%1 + 104], r13
    mov [%1 + 112], r14
    mov [%1 + 120], r15
    mov [%1 + 128], rflags
%endmacro
