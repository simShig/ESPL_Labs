
segment .data

    x_struct: dd 6
    x_val: db 1,0xf0,1,2,0x44,0x4f
    y_struct: dd 5
    y_val: db 1,1,2,0x44,1
    carry: db 0
    ans: dd 0

    ; format  db "%2x",10,0
    format      db      "0x%02hhx",0x0a,0



segment .text
        global  main                   ; let the linker know about main
        extern  printf, malloc         ; resolve printf from libc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

main:
        push    ebp             ; prepare stack frame for main
        mov     ebp, esp
        
        ; mov     eax, x_struct ;testing minmax
        ; mov     ebx, y_struct
        ; call    get_max_min


        push    y_struct
        push    x_struct
        call    add_multi

        ; add     esp, 8
        
        ; mov     esi, dword [ans]
        ; mov     esi, dword [esi]
        
        ; push    dword esi
        ; push    format
        ; call    printf

        ; add     esp, 8

        

        leave                   ;end main convention
        ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

get_max_min:
        push    ebp             ; prepare stack frame for main
        mov     ebp, esp

        mov     edi, eax  ;x_struct
        mov     esi, ebx  ;y_struct

        mov     ecx, [edi]  ;extracting lengths
        mov     edx, [esi]

        cmp     ecx, edx

        jns     negative   ;if not negative => [esi]>[edi] and we need to flip
        mov     eax, esi
        mov     ebx, edi
    negative:

        mov     esp, ebp        ; restore stack frame
        pop     ebp
        ret                     ; return


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


add_multi:
        push    ebp             ; prepare stack frame for main
        mov     ebp, esp
        
        mov     edi, [ebp + 8]
        mov     esi, [ebp + 12]

        mov     eax, edi
        mov     ebx, esi
        call    get_max_min     
        mov     edi, eax        ;now edi holds pointer to the longer struct and esi to the shorter one
        mov     esi, ebx        
        
        mov     eax, dword [edi]
        add     eax, 1          ;adding place for ans_struct

        push    eax
        call    malloc
        pop     ebx             ;restore stack

        mov    dword [ans], eax


        ;eax has a pointer to the allocated memmory
        mov     ebx, dword [edi]
        mov     [eax], ebx

        add     eax, 4          ;make eax point to the first element of ans_num 

        mov     ebx, 0              ;accumulator
        mov     ecx, dword [edi]    ;counter
        mov     edx, dword [esi]    ;length of shorter array
        add     edi, 4          ;pointers to arrays
        add     esi, 4

        

    start_loop1:
        add     bl, byte [edi]

        cmp     byte [carry], 0
        jz      no_carry
        add     bl, 1           ;adding carry
      no_carry:   
        cmp     edx, 0
        js      arr2_ended
        add     bl, byte [esi]  ;adding short[i]

        jnc     carry_0
        mov     byte [carry], 1
      carry_0:
        jc      arr2_ended
        mov     byte [carry], 0
      arr2_ended:   


        mov     [eax], bl
        xor     ebx, ebx

        inc     eax
        inc     edi
        inc     esi
        dec     ecx
        dec     edx

        cmp     ecx, 0
        jnz     start_loop1

    end_loop1:

        sub     eax, 6


        mov     edi, eax

        push    dword [edi]
        push    format
        call    printf

        inc     edi
        push    dword [edi]
        push    format
        call    printf

        inc     edi
        push    dword [edi]
        push    format
        call    printf

        inc     edi
        push    dword [edi]
        push    format
        call    printf

        inc     edi
        push    dword [edi]
        push    format
        call    printf

        inc     edi
        push    dword [edi]
        push    format
        call    printf



        mov     esp, ebp        ; restore stack frame
        pop     ebp
        ret                     ; return


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;