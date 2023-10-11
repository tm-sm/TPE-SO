GLOBAL cpuVendor
GLOBAL keydown
GLOBAL keypress
GLOBAL interrupt
GLOBAL clock
GLOBAL play_sound
GLOBAL stop_sound
GLOBAL timer_wait
GLOBAL get_ip
GLOBAL get_stack_trace
GLOBAL switch_process

EXTERN displayRegs

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

load_process:
    mov rsp, rcx
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
    push rax
    mov rax, [rsp + 8]
    ret

switch_process:
    ;goes to the new stack, and sets the rip as the return point
    push rax
    mov rax, rsp
    mov rsp, [rdi + 16]

    push qword[rdi]

    mov rsp, rax
    pop rax

    ;saves the rsp
    push qword[rdi + 16]

    ;loads proc->stack to registers
    mov rax, [rdi + 24]
    push qword[rax]
    mov rbx, [rax + 8]
    mov rcx, [rax + 16]
    mov rdx, [rax + 24]
    mov rbp, [rax + 32]
    mov rdi, [rax + 40]
    mov rsi, [rax + 48]
    mov r8, [rax + 56]
    mov r9, [rax + 64]
    mov r10, [rax + 72]
    mov r11, [rax + 80]
    mov r12, [rax + 88]
    mov r13, [rax + 96]
    mov r14, [rax + 104]
    mov r15, [rax + 112]
    pop rax

    ;changes the stack and moves to the new rip
    pop rsp
    sub rsp, 8
    ret

get_stack_trace:
    mov rax, rsp
    ret