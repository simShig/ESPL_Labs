section .rodata
    str1: db "%x",10,0
    str2: db "%d",10,0
    x_struct: dd 5
    x_num: db 0xaa, 1,2,0x44,0x4f   

segment .text
global main
extern print_multi

 main:
    push    ebp                 
    mov     ebp, esp  

    push dword x_struct
    call print_multi

    mov eax, 0
    mov     esp, ebp
    pop     ebp
    ret  