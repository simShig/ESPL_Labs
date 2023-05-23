section .rodata
    str1: db "%d",10,0
    str2: db "%s",10,0


segment .text
global main
extern printf
extern puts

main:
    push    ebp                 
    mov     ebp, esp        
    push dword[ebp+8]
    push dword str1
    call printf
    add esp, 8

    mov     esi, [ebp + 12] 
    mov     ebx, [esi] 
    push dword ebx
    call puts
    

    mov     eax, 0      ; return(0)
    mov     esp, ebp    ; pop
    pop     ebp             ; stack frame
    ret

