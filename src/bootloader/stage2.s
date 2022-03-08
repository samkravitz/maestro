; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: stage2.s
; DATE: March 3rd, 2022
; DESCRIPTION: stage 2 bootloader

[bits 16]
org 0x5000                     ; we are expecting stage1 to load us to memory address 0x5000

mov sp, 5000h                  ; stack now points to here

; detect memory map
; we will store the generated list at address 0x4000 (es:di => 0000:4000)
memory_map:
xor ax, ax                     ; ax = 0
mov es, ax                     ; es = 0
mov di, 0x4000                 ; es:di => 0000:4000

first_call:
xor ebx, ebx                   ; ebx = 0
mov edx, 0x534d4150            ; magic number
mov ax, 0xe820                 ; needed for int 15
mov ecx, 24                    ; tells BIOS to give a 24 byte entry
int 0x15
cmp eax, 0x534d4150            ; magic number should be in eax after first call
jne .error

.loop:
test ebx, ebx
jz .done                       ; done detecting memory when ebx = 0
mov eax, 0xe820
mov ecx, 24
add di, 24                     ; increment di by 24 bytes
int 0x15
jmp .loop

.error:
mov si, memory_map_error
call puts
jmp $

.done:

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

jmp 0x08:pmode                 ; jump to protected mode!

; 32 bit protected mode
pmode:
[bits 32]

jmp KERNEL_ADDR

; tell nasm remainder of this file is 16 bit mode
[bits 16]

; initialize gdt
gdt:
gdt_null:           ; null descriptor
    dd 0x0          ; 4 bytes of 0
    dd 0x0          ; 4 bytes of 0

gdt_code:           ; code segment descriptor
    dw 0xffff		; limit (bits 0-15)
    dw 0x0          ; base  (bits 0-15)
    db 0x0          ; base  (bits 16-23)
    db 10011010b    ; flags
    db 11001111b    ; flags cont., limit (bits 16-19)
    db 0x0          ; base (bits 24-31)

gdt_data:           ; data segment descriptor
    dw 0xffff       ; limit (bits 0-15)
    dw 0x0          ; base (bits 0-15)
    db 0x0          ; base (bits 16-23)
    db 10010010b    ; flags
    db 11001111b    ; flags cont., limit (bits 16-19)
    db 0x0          ; base (bits 24-31)
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
 
    mov di, 0x0500
    mov si, 0x0510
 
    mov al, byte [es:di]
    push ax
 
    mov al, byte [ds:si]
    push ax
 
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xff
 
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
KERNEL_ADDR equ 10000h ; address of kernel
