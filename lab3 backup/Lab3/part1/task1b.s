section .data
    char db 0
    inFile dd 0
    outFile dd 1
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

encoder:
    read_char:
        mov eax, SYS_READ
        mov ebx, dword[inFile]
        mov ecx, char
        mov edx, 1    ;eax = strlen(ecx) = strlen(argv[i])
        int 0x80
    check_eof:
        cmp eax, 0
        jle exit_program
    should_encode:
        cmp byte[char], 'A'
        jl printf_char
        cmp byte[char], 'z'
        jg printf_char
        add byte[char], 1
    printf_char:
        mov eax, SYS_WRITE
        mov ebx, dword[outFile]
        mov ecx, char
        mov edx, 1    ;eax = strlen(ecx) = strlen(argv[i])
        int 0x80
    jmp encoder

    exit_program:
        mov eax, SYS_EXIT
        mov ebx, 0
        int 0x80


