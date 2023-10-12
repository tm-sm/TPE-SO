GLOBAL cpuVendor
GLOBAL keydown
GLOBAL keypress
GLOBAL interrupt
GLOBAL clock
GLOBAL play_sound
GLOBAL stop_sound
GLOBAL timer_wait
GLOBAL get_ip
GLOBAL prepare_process

EXTERN displayRegs
EXTERN cPrintHex
EXTERN cNewline

section .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	pushf
%endmacro

%macro popState 0
    popf
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro pushStateInverse 0
    push rsp
    pushf
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rsi
	push rdi
	push rbp
	push rdx
	push rcx
	push rbx
	push rax
%endmacro

%macro popStateInverse 0
    pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rbp
	pop rdi
	pop rsi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	popf
	pop rsp
%endmacro

clock:
	push rbp
	mov rbp, rsp

	mov al, dil
	out 70h, al
	xor rax, rax
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

keydown:
    push rbp
    mov rbp, rsp
    in al, 0x64
    and al, 0x01
    jz .not_pressed
    in al, 0x60
    jmp .finish
.not_pressed:
    mov al, 0
.finish:
    leave
    ret

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

interrupt:
    pushstate
    int 80h
    popstate
    ret

play_sound:
	; code adapted from https://wiki.osdev.org/PC_Speaker
    push rbp
    mov rbp, rsp
    push rbx
    push rdx

    mov ah, 2
    mov dl, 7
    int 21h

    mov al, 0xB6
    out 0x43, al

    mov ebx, edi
    mov al, byte [ebx]
    out 0x42, al

    shr ebx, 8
    mov al, byte [ebx]
    out 0x42, al

    in al, 0x61
    mov bl, al

    or bl, 3
    cmp al, bl
    je .finish

    mov al, bl
    out 0x61, al

.finish:
    pop rdx
    pop rbx
    mov rsp, rbp
    pop rbp
    ret

stop_sound:
    push rbp
    mov rbp, rsp

    in al, 0x61
    and al, 0xFC
    out 0x61, al

    mov rsp, rbp
    pop rbp
    ret

timer_wait:
    push rbp
    mov rbp, rsp

    mov eax, dword [rbp+16]    ; recibe el tiempo que espera
    mov ebx, eax

    ; calcula numero de ciclos que tiene que esperar
    mov ecx, 0
    mov edx, 1000000
    mul edx
    mov ecx, eax
    xor edx, edx
    mov eax, ebx
    mul edx
    add eax, ecx

.wait_loop:
    rdtsc
    cmp rax, rdx
    jb .wait_loop

    pop rbp
    ret

get_ip:
    mov rax, [rbp + 8]
    ret

prepare_process:
    push rbp
    mov rbp, rsp

    mov dword[rdi - 2], 0x0      ;Stack Segment
    mov qword[rdi - 10], rsi     ;RSP
    mov qword[rdi - 18], 0x202   ;RFLAGS
    mov dword[rdi - 20], 0x8    ;CS
    mov qword[rdi - 28], rdx     ;RIP
    mov qword[rdi - 36], 0x2     ;RAX
    mov qword[rdi - 44], 0x3     ;RBX
    mov qword[rdi - 52], 0x4     ;RCX
    mov qword[rdi - 60], 0x5     ;RDX
    mov qword[rdi - 68], rdi     ;RBP
    mov qword[rdi - 76], 0x6     ;RDI
    mov qword[rdi - 80], 0x7     ;RSI
    mov qword[rdi - 88], 0x8     ;R8
    mov qword[rdi - 96], 0x9    ;R9
    mov qword[rdi - 104], 0x10    ;R10
    mov qword[rdi - 112], 0x11    ;R11
    mov qword[rdi - 120], 0x12    ;R12
    mov qword[rdi - 128], 0x13    ;R13
    mov qword[rdi - 136], 0x14    ;R14
    mov qword[rdi - 144], 0x15    ;R15

    mov rsp, rbp
    pop rbp
    ret

get_stack_trace:
    mov rax, rsp
    ret