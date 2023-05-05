section .data
    input_file db 0
    output_file db 0
    input_buffer db 1
    output_buffer db 1
    newline db 0xA

section .text
global main

; File descriptors numbers
%define STDIN 0
%define STDOUT 1
%define STDERR 2
; Syscall numbers
%define SYS_READ 0x3
%define SYS_WRITE 0x4
%define SYS_OPEN 0x5
%define SYS_CLOSE 0x6
%define SYS_EXIT 0x1

; File open flags
%define O_RDONLY 0x0
%define O_WRONLY 0x1
%define O_CREAT 0x40
%define O_TRUNC 0x200
extern strlen
main:
    ; Parsing command-line arguments
    ;edi = argc,esi = argv
    mov edi, dword[esp+4]
    mov esi, dword[esp+8]

    loop_argv:
        mov ecx, dword[esi]
    get_argument_length:
        push ecx
        call strlen     ;eax = strlen(ecx) = strlen(argv[i])
        pop ecx
    print_argument:
        mov edx, eax    ;eax = strlen(ecx) = strlen(argv[i])
        mov eax, SYS_WRITE
        mov ebx, STDOUT 
        mov ecx, ecx
        int 0x80
        mov eax, SYS_WRITE
        mov ebx, STDOUT
        mov ecx, newline
        mov edx, 1    ;eax = strlen(ecx) = strlen(argv[i])
        int 0x80
    continue_loop:
        sub edi, 1
        add esi, 4
        cmp edi, 0
        jne loop_argv

    exit_program:
        mov eax, SYS_EXIT
        mov ebx, 0
        int 0x80


