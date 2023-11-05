GLOBAL timer_wait
GLOBAL get_ip
GLOBAL prepare_process
GLOBAL interruptTick
GLOBAL enterSem
GLOBAL exitSem
EXTERN interruptTick
EXTERN displayRegs
EXTERN cPrintHex
EXTERN cNewline
EXTERN roundRobin
EXTERN raiseUserInterruptedFlag

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
    mov rsp, rdi

    push dword 0x0      ;SS
    push rdi            ;RSP
    push qword 0x202    ;RFLAGS
    push dword 0x8      ;CS
    push rsi            ;RIP
    push qword 0x14     ;R15
    push qword 0x13     ;R14
    push qword 0x12     ;R13
    push qword 0x11     ;R12
    push qword 0x10     ;R11
    push qword 0x9      ;R10
    push qword 0x8      ;R9
    push qword 0x7      ;R8
    push rcx            ;RSI
    push rdx            ;RDI
    push rdi            ;RBP
    push qword 0x3      ;RDX
    push qword 0x2      ;RCX
    push qword 0x1      ;RBX
    push qword 0x0      ;RAX

    mov rax, rsp

    mov rsp, rbp
    pop rbp
    ret

get_stack_trace:
    mov rax, rsp
    ret
enterSem: ;semaforo de los semaforos
    mov edx,1
    xchg edx,dword [rdi]
    cmp edx,0
    je isfree
    jmp enterSem ;Spinlock ("legal")
    isfree:
    ret

exitSem:
    mov dword [rdi],0
    ret

interruptTick:
    call raiseUserInterruptedFlag
    int 20h
    ret
