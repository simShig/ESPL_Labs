
//~~~~~~~~~~~~~~~~~part 1-A ~~~~~~~~~~~~~~~~~~~~~~

@ section .data
@     cmdline db "Command line arguments:", 0xA
@     cmdline_len equ $-cmdline

@ section .text
@     global _start

@ _start:
@     ; get the number of command line arguments
@     mov eax, [esp+4]    ; get argc from the stack
@     lea ebx, [esp+8]    ; set ebx to point to the first argument on the stack
    
@     ; print the command line arguments
@     .loop:
@         push ebx          ; save ebx
@         push ebx          ; pass the argument to strlen
@         call strlen       ; call strlen to get the length of the current argument
@         add esp, 4        ; clean up the stack
@         mov edx, eax      ; set edx to the length of the current argument
@         mov ecx, ebx      ; set ecx to point to the current argument
@         mov ebx, 2        ; file descriptor for stderr
@         mov eax, 4        ; system call number for "sys_write"
@         int 0x80          ; call "sys_write" to write the current argument to stderr
@         mov edx, cmdline_len ; set edx to the length of the message to print
@         mov ecx, cmdline  ; set ecx to point to the message to print
@         mov ebx, 2        ; file descriptor for stderr
@         mov eax, 4        ; system call number for "sys_write"
@         int 0x80          ; call "sys_write" to print the message to stderr
@         pop ebx           ; restore ebx
@         add ebx, eax      ; move ebx to point to the next argument
@         cmp dword [ebx], 0 ; check if we've reached the end of the arguments
@         jne .loop         ; if not, continue looping
    
@     ; exit the program
@     mov eax, 1           ; system call number for "sys_exit"
@     xor ebx, ebx         ; set ebx to 0 for "sys_exit"
@     int 0x80             ; call "sys_exit" to exit the program


    //~~~~NOT needed -imported from the util.c~~~


@ ; strlen function         
@ strlen:
@     push ebp
@     mov ebp, esp
@     xor eax, eax        ; set eax to 0
@     .loop:
@         cmp byte [ecx+eax], 0 ; check if we've reached the end of the string
@         je .done             ; if so, jump to the end of the function
@         inc eax              ; otherwise, increment eax and continue looping
@         jmp .loop
@     .done:
@         mov esp, ebp
@         pop ebp
@         ret


//~~~~~~~~~~~~~~~~~~~~~~part1-B~~~~~~~~~~~~~~~~
@ section .data
@     cmdline db "Command line arguments:", 0xA
@     cmdline_len equ $-cmdline
@     Infile dd 0
@     Outfile dd 1

@ section .text
@     global _start

@ _start:
@     ; get the number of command line arguments
@     mov eax, [esp+4]    ; get argc from the stack
@     lea ebx, [esp+8]    ; set ebx to point to the first argument on the stack
    
@     ; print the command line arguments
@     .loop:
@         push ebx          ; save ebx
@         push ebx          ; pass the argument to strlen
@         call strlen       ; call strlen to get the length of the current argument
@         add esp, 4        ; clean up the stack
@         mov edx, eax      ; set edx to the length of the current argument
@         mov ecx, ebx      ; set ecx to point to the current argument
@         mov ebx, 2        ; file descriptor for stderr
@         mov eax, 4        ; system call number for "sys_write"
@         int 0x80          ; call "sys_write" to write the current argument to stderr
@         mov edx, cmdline_len ; set edx to the length of the message to print
@         mov ecx, cmdline  ; set ecx to point to the message to print
@         mov ebx, 2        ; file descriptor for stderr
@         mov eax, 4        ; system call number for "sys_write"
@         int 0x80          ; call "sys_write" to print the message to stderr
@         pop ebx           ; restore ebx
@         add ebx, eax      ; move ebx to point to the next argument
@         cmp dword [ebx], 0 ; check if we've reached the end of the arguments
@         jne .loop         ; if not, continue looping
    
@     ; set up Infile and Outfile global variables
@     mov dword [Infile], 0 ; stdin
@     mov dword [Outfile], 1 ; stdout
    
@     ; read from stdin, encode, and write to stdout
@     call encode
    
@     ; exit the program
@     mov eax, 1           ; system call number for "sys_exit"
@     xor ebx, ebx         ; set ebx to 0 for "sys_exit"
@     int 0x80             ; call "sys_exit" to exit the program

@ ; encode function
@ encode:
@     ; set up variables
@     mov esi, Infile     ; set esi to point to Infile
@     mov edi, Outfile    ; set edi to point to Outfile
@     xor eax, eax        ; clear eax
    
@     ; read from stdin, encode, and write to stdout
@     .loop:
@         ; read from stdin
@         mov ebx, [esi]    ; set ebx to Infile
@         mov ecx, esp      ; set ecx to point to the top of the stack
@         mov edx, 1024     ; set edx to the number of bytes to read
@         mov eax, 3        ; system call number for "sys_read"
@         int 0x80          ; call "sys_read" to read from stdin
@         cmp eax, 0        ; check if we've reached the end of the input
@         je .done          ; if so, exit the loop
        
@         ; encode the input
@         xor ebx, ebx      ; set ebx to 0 for the loop counter
@         .encode_loop:
@             cmp byte [esp+ebx], 0 ; check if we've reached the end of the current string
@             je .done_encoding     ; if so, exit


//~~~~~~~~~~~~~~~~~~part 1b ~~~~~~~~~~~~~~~~

section .data
    cmdline db "Command line arguments:", 0xA
    cmdline_len equ $-cmdline
    Infile dd 0
    Outfile dd 1
    buf resb 1

section .text
    global _start

_start:
    ; get the number of command line arguments
    mov eax, [esp+4]    ; get argc from the stack
    lea ebx, [esp+8]    ; set ebx to point to the first argument on the stack
    
    ; print the command line arguments
    .loop:
        push ebx          ; save ebx
        push ebx          ; pass the argument to strlen
        call strlen       ; call strlen to get the length of the current argument
        add esp, 4        ; clean up the stack
        mov edx, eax      ; set edx to the length of the current argument
        mov ecx, ebx      ; set ecx to point to the current argument
        mov ebx, 2        ; file descriptor for stderr
        mov eax, 4        ; system call number for "sys_write"
        int 0x80          ; call "sys_write" to write the current argument to stderr
        mov edx, cmdline_len ; set edx to the length of the message to print
        mov ecx, cmdline  ; set ecx to point to the message to print
        mov ebx, 2        ; file descriptor for stderr
        mov eax, 4        ; system call number for "sys_write"
        int 0x80          ; call "sys_write" to print the
; encode function
; encodes a character read from stdin by adding 1 to its ASCII value
; if it is in the range 'A' to 'z', and outputs it to stdout
; arguments:
;   none
; returns:
;   none
encode:
    ; read a character from stdin
    mov eax, 3        ; system call number for "sys_read"
    mov ebx, dword [Infile] ; file descriptor for stdin
    mov ecx, buf      ; buffer to read into
    mov edx, 1        ; number of bytes to read
    int 0x80          ; call "sys_read" to read a byte from stdin

    ; check if we've reached the end of input
    cmp eax, 0        ; if eax is 0, we've reached the end of input
    je .done          ; jump to .done if we've reached the end of input

    ; check if the byte is in the range 'A' to 'z'
    cmp byte [buf], 'A' ; check if the byte is >= 'A'
    jl .write          ; if not, jump to .write
    cmp byte [buf], 'z' ; check if the byte is <= 'z'
    jg .write          ; if not, jump to .write

    ; encode the byte by adding 1 to its value
    add byte [buf], 1

    ; write the encoded byte to stdout
    .write:
        mov eax, 4        ; system call number for "sys_write"
        mov ebx, dword [Outfile] ; file descriptor for stdout
        mov ecx, buf      ; buffer to write
        mov edx, 1        ; number of bytes to write
        int 0x80          ; call "sys_write" to write the encoded byte to stdout

    ; jump back to the start of the loop
    jmp encode

    ; exit the loop
    .done:
        ret
