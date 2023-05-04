section .data
    hello db 'hello world',0xA ; define the string to be printed and add linefeed character

section .text
    global _start           ; entry point of the program

_start:
    ; write string to stdout
    mov     eax, 4          ; system call for write
    mov     ebx, 1          ; file descriptor for stdout
    mov     ecx, hello      ; address of the string to be printed
    mov     edx, 13         ; length of the string
    int     0x80            ; invoke system call

    ; exit program
    mov     eax, 1          ; system call for exit
    xor     ebx, ebx        ; return value of 0
    int     0x80            ; invoke system call
