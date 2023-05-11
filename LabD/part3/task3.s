section .rodata
    str2: dw "0x%02hhx",10,0
section .data
    STATE: dd 0x1010 
    MASK: dd 0xffff

segment .text
global rand_num
global main
extern printf

main:
    push    ebp                 
    mov     ebp, esp  

    push dword [STATE]
    push str2
    call printf ;print state befor iterations
    add esp, 8

    mov ecx, 20 ;loop counter
    main_loop:
        push ecx 
        call rand_num
        push eax
        push str2
        call printf
        add esp, 8

        pop ecx
        sub ecx, 1
        jz finish1

        jmp main_loop

   

    finish1:
        mov     esp, ebp
        pop     ebp
        ret 
            
rand_num:
    push ebp
    mov ebp, esp
    mov ax, [STATE]
    mov bX, [MASK]

    xor bx, ax
    jp even_ones
    

    STC ;cf = 1
    RCR ax,1
    jmp finish_rans_num

    even_ones:
        shr ax,1
        jmp finish_rans_num

    finish_rans_num:   
        mov [STATE] ,ax
        mov  eax, [STATE]
        mov     esp, ebp
        pop     ebp
        ret  