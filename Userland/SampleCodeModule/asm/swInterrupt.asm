GLOBAL interrupt

section .text

interrupt:
    int 80h
    ret