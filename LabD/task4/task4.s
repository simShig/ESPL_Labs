section .rodata
    str2: dw "state : %02hhx",10,0
section .data
    STATE: dd 0x1010 
    MASK: dd 0xffff


;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
segment .text
global rand_num
global main
extern printf
section .data
    msg_invalid_argument db "Invalid argument.", 0
    msg_usage db "Usage: ./your_program [-I | -R]", 0

section .text
    extern printf
    extern strcmp
    global main

main:
    ; Check if there are command-line arguments
    cmp dword [argc], 1
    jle default_operation  ; No arguments provided, perform default operation

    ; Get the first command-line argument (argv[1])
    mov ebx, [argv]
    add ebx, 4

    ; Compare the argument with "-I"
    mov edi, ebx
    mov esi, "-I"
    call strcmp
    cmp eax, 0
    je input_operation  ; Argument is "-I"

    ; Compare the argument with "-R"
    mov edi, ebx
    mov esi, "-R"
    call strcmp
    cmp eax, 0
    je random_operation  ; Argument is "-R"

    ; Invalid argument
    push msg_invalid_argument
    call printf
    jmp end

default_operation:
    ; Perform default operation
    ; ...

    jmp end

input_operation:
    ; Perform operation using numbers from stdin
    ; ...

    jmp end

random_operation:
    ; Perform operation using the Pseudo-Random number generator
    ; ...

end:
    ; Exit the program
    mov eax, 0
    ret



;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
main:
    push    ebp                 
    mov     ebp, esp  

    push dword [STATE]
    push str2
    call printf ;print state befor iterations
    add esp, 8

    mov ecx, 10 ;loop counter - 10 iterations
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
