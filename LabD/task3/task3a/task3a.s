segment .text
global get_max_min
get_max_min:
    push    ebp                 
    mov     ebp, esp  

    mov ecx, [eax] ;ecx = eax.size
    mov edx, [ebx] ;ecx = ebx.size

    cmp ecx, edx
    jg finish ;cx > dx

    mov ecx, ebx
    mov ebx, eax
    mov eax, ecx

    finish:
    mov     esp, ebp
    pop     ebp
    ret    
    
