
section .rodata
    str1: db "x_struct:",10,0
    str2: db "y_struct:",10,0
    str3: db "x_struct + y_struct:",10,0
    x_struct: dd 6
    x_val: db 1,0xf0,1,2,0x44,0x4f
    y_struct: dd 5
    y_val: db 1,1,2,0x44,1

segment .text
global main
global add_multi
extern get_max_min
extern print_multi
extern printf

main:
    push    ebp                 
    mov     ebp, esp  

    push dword str1
    call printf
    add esp, 4
    push dword x_struct
    call print_multi

    push dword str2
    call printf
    add esp, 4
    push dword y_struct
    call print_multi

    push dword str3
    call printf
    add esp, 4
    call add_multi
    push dword eax
    call print_multi

    mov edi, eax
    extern free
    call free

    mov eax, 0
    mov     esp, ebp
    pop     ebp
    ret    

add_multi:
    push    ebp  
    mov     ebp, esp  
    ;enter func

    mov eax, [ebp+8] ;p1
    mov ebx, [ebp+12] ;p2
    call get_max_min
    mov ecx, ebx ;moving p1 and p2 since malloc writes to eax
    mov ebx, eax 
    mov edi, [ebx]
    add edi, 4
    
    push dword ebx
    push dword ecx
    push dword edi

    extern malloc
    call malloc ;takes input from edi, returns pointer to eax
    ; now new p3 = eax, p1 (bigger) = ebx, p2 (smaller) = ecx
    
    pop dword edi
    pop dword ecx
    pop dword ebx

    push dword eax ; store p3 pointer for return
    sub edi, 4 ; edi = max size
    mov dword [eax], edi ;put new arrays size of p3 in its place
    mov esi, [ebx] 
    mov edi, [ecx] ; edi = min size
    sub esi, edi ; esi = max size - min size
    add eax, 4
    add ebx, 4
    add ecx, 4 ;move all pointers to start of array

    CLC ;clears carry flag
    inc edi
    inc esi
    
    loop:
        dec edi
        jz only_max_left

        mov dl,byte [ebx]
        adc dl,byte [ecx]
        mov byte[eax], dl

        inc eax
        inc ebx
        inc ecx
        jmp loop

    only_max_left:
        dec esi
        jz finish

        mov dl,byte [ebx]
        adc dl, 0
        mov byte[eax], dl ; add element with previous carry
        inc ebx
        inc eax

        jmp only_max_left

    finish:

    pop eax

    ;exit func
    mov     esp, ebp
    pop     ebp
    ret  



