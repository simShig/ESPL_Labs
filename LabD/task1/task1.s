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
    mov edi, [ebp+8] ;argc

    push dword[ebp+8]
    push dword str1
    call printf
    add esp, 8


    mov esi, [ebp + 12] ; start argv0

    
    loop:
        cmp edi, 0
        jz post_loop
        
        sub edi, 1
    
        push dword [esi]
        call puts
        add esi, 4


        add esp, 4
        jmp loop

    post_loop:
        mov eax, 0
        mov     esp, ebp
        pop     ebp
        ret    

    
