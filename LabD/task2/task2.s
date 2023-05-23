section .rodata
    str1: db "%02hhx",10,0
    str2: db "%d",10,0
    x_struct: dd 5
    x_num: db 0xaa, 1,2,0x44,0x4f    

segment .text
global print_multi
; global main
extern printf

print_multi:
    push    ebp                 
    mov     ebp, esp  
    mov edi, [ebp+8] ;p
    mov esi, [edi] ; size
    add edi, 4

    mov ebx, edi ; pointer to arr[0]
    
    loop:
        cmp esi, 0
        jz post_loop
        
        sub esi, 1

        mov ecx,0
        mov cl, [ebx]
        push dword ecx
        push dword str1

        call printf
        add ebx, 1

        add esp, 8
        jmp loop

    post_loop:
        mov     esp, ebp
        pop     ebp
        ret    

    
