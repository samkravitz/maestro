; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: stage2.s
; DATE: March 3rd, 2022
; DESCRIPTION: stage 2 bootloader

[bits 16]
org 5000h                      ; we are expecting stage1 to load us to memory address 5000h

mov sp, 5000h                  ; stack now points to here

; detect memory map
; we will store the generated list at address 4000h (es:di => 0000:4000)
memory_map:
xor ax, ax                     ; ax = 0
mov es, ax                     ; es = 0
mov di, 4000h                  ; es:di => 0000:4000

first_call:
xor ebx, ebx                   ; ebx = 0
mov edx, 534d4150h             ; magic number
mov ax, 0e820h                 ; needed for int 15
mov ecx, 24                    ; tells BIOS to give a 24 byte entry
int 15h
cmp eax, 534d4150h             ; magic number should be in eax after first call
jne .error

.loop:
test ebx, ebx
jz .done                       ; done detecting memory when ebx = 0
mov eax, 0e820h
mov ecx, 24
add di, 24                     ; increment di by 24 bytes
int 15h
jmp .loop

.error:
mov si, memory_map_error
call puts
jmp $

.done:

mov [es:di],     dword 0xcafebabe  ; magic number so the kernel knows where the memory map ends

; next we will verify that a20 line is enabled before jumping to protected mode
call check_a20
cmp ax, 1                      ; ax = 1 if a20 is enabled
jne a20_disabled_error         ; TODO - handle case when a20 is not enabled

; now we know a20 is enabled, we can get ready to enter protected mode
cli
lgdt [gdt_descriptor]          ; load gdt
mov eax, cr0                   ; eax = cr0
or eax, 1                      ; enable protected mode
mov cr0, eax                   ; cr0 = eax

jmp 8h:pmode                   ; jump to protected mode!

; 32 bit protected mode
pmode:
[bits 32]

; the first thing to do in protected mode is to
; move kernel from where it was loaded by stage1 (10000h)
; to where it is expected by the linker to be (100000h)
; 
; this moves 4M for the kernel which is plenty, but if the
; kernel ever takes up more than 4M, this will need to be updated
mov esi, KERNEL_LOAD_BASE
mov edi, KERNEL_NEW_BASE
mov ecx, 400000h                ; move 4M of memory
rep movsb

jmp KERNEL_NEW_BASE

; tell nasm remainder of this file is 16 bit mode
[bits 16]

; initialize gdt
gdt:
gdt_null:           ; null descriptor
    dd 0            ; 4 bytes of 0
    dd 0            ; 4 bytes of 0

gdt_code:           ; code segment descriptor
    dw 0ffffh       ; limit (bits 0-15)
    dw 0            ; base  (bits 0-15)
    db 0            ; base  (bits 16-23)
    db 10011010b    ; flags
    db 11001111b    ; flags cont., limit (bits 16-19)
    db 0            ; base (bits 24-31)

gdt_data:           ; data segment descriptor
    dw 0ffffh       ; limit (bits 0-15)
    dw 0            ; base (bits 0-15)
    db 0            ; base (bits 16-23)
    db 10010010b    ; flags
    db 11001111b    ; flags cont., limit (bits 16-19)
    db 0            ; base (bits 24-31)
gdt_end:

; 6 byte value to be stored in gdtr
gdt_descriptor:
dw gdt_end - gdt - 1    ; size of gdt minus 1
dd gdt                  ; starting address of GDT

; poll if a20 line is enabled
; inputs - none
; ax = 1 if a20 is enabled, otherwise ax = 0
; routine adopted from https://wiki.osdev.org/A20_Line
check_a20:
    pushf
    push ds
    push es
    push di
    push si
 
    cli
 
    xor ax, ax                     ; ax = 0
    mov es, ax
 
    not ax                         ; ax = 0xffff
    mov ds, ax
 
    mov di, 0500h
    mov si, 0510h
 
    mov al, byte [es:di]
    push ax
 
    mov al, byte [ds:si]
    push ax
 
    mov byte [es:di], 00h
    mov byte [ds:si], 0ffh
 
    cmp byte [es:di], 0xff
 
    pop ax
    mov byte [ds:si], al
 
    pop ax
    mov byte [es:di], al
 
    mov ax, 0
    je .exit
 
    mov ax, 1
 
    .exit:
    pop si
    pop di
    pop es
    pop ds
    popf
 
    ret

a20_disabled_error:
    mov si, a20_not_enabled
    call puts
    jmp $

%include 'puts.inc'

hello: db 'hello from stage2!', 0
memory_map_error: db 'error detecting memory map', 0
a20_not_enabled: db 'a20 is not enabled!', 0
welcome_pmode: db 'welcome to protected mode!', 0

; constants
MMAP_ADDR        equ  4000h  ; address of memory map
KERNEL_LOAD_BASE equ 10000h  ; address where kernel was loaded by stage1
KERNEL_NEW_BASE  equ 100000h ; address where linker expects kernel to be loaded by stage2 (1M)
