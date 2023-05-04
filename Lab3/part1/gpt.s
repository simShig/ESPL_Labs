.section .data
input_file db 0
output_file db 0
input_buffer db 1
output_buffer db 1

section .text
global _start

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

; Function prototypes
extern read
extern write
extern open
extern close

_start:
    ; Parsing command-line arguments
    pop ebx ; argc
    pop ebx ; argv[0]
    pop ebx ; argv[1]
    cmp byte [ebx], '-'
    jne open_input_file
    cmp byte [ebx+1], 'i'
    jne invalid_arguments
    mov input_file, ebx+2
    pop ebx ; argv[2]
    cmp byte [ebx], '-'
    jne open_output_file
    cmp byte [ebx+1], 'o'
    jne invalid_arguments
    mov output_file, ebx+2
    pop ebx ; argv[3]
    jmp check_end_of_arguments

open_input_file:
    mov input_file, ebx
    jmp check_end_of_arguments

open_output_file:
    mov output_file, ebx
    jmp check_end_of_arguments

check_end_of_arguments:
    cmp ebx, 0
    je open_files

    ; Getting next command-line argument
    pop ebx
    cmp byte [ebx], '-'
    je invalid_arguments
    cmp byte [ebx-1], 'i'
    je open_input_file
    cmp byte [ebx-1], 'o'
    je open_output_file
    jmp check_end_of_arguments

open_files:
    ; Opening input file
    mov eax, SYS_OPEN
    mov ebx, input_file
    mov ecx, O_RDONLY
    xor edx, edx
    int 0x80
    mov esi, eax ; save input file descriptor

    ; Opening output file
    mov eax, SYS_OPEN
    mov ebx, output_file
    mov ecx, O_CREAT | O_WRONLY | O_TRUNC
    mov edx, 0644
    int 0x80
    mov edi, eax ; save output file descriptor

    ; Encoding input and writing to output
    xor ecx, ecx ; clear ecx
.read_loop:
    ; Reading a character from input
    mov eax, SYS_READ
    mov ebx, esi
    mov ecx, input_buffer
    mov edx, 1
    int 0x80
    cmp eax, 0
    je .end_program

    ; Encoding the character
    mov al, [input_buffer]
    cmp al, 'A'
    jl .write_output
    cmp al, 'z'
    jg .write_output
    add al, 1

.write_output:
    ; Writing the character to output
    mov eax, SYS_WRITE
    mov ebx, edi
    mov ecx, output_buffer
    mov edx, 1
    int 0x80
    cmp eax, 0
    je .end_program
    jmp .read_loop

.end_program:
   
