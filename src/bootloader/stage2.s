; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: stage2.s
; DATE: March 3rd, 2022
; DESCRIPTION: stage 2 bootloader
;    detects the physical memory map,
;    moves the kernel to 1M physical,
;    maps the kernel to 0xc0000000 virtual,
;    enables paging,
;    & jumps to the kernel
;
;     MEMORY MAP
; 0x5000 : 0x5200  - stage2 bootloader
; 0x8000 : 0x9000  - page directory
; 0x9000 : 0xA000  - identity page table
; 0xA000 : 0xB000  - kernel page table
; 
; 0x100000 : ?     - kernel

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

; set VESA mode 118h
; 1024x768, 24bpp, 3072 pitch
; TODO - mode 118h may not always have these properties, and may be unavailable
video_mode:
;mov ax, 4F02h                   ; set vbe mode
;mov bx, 4118h                   ; vbe mode number
;int 0x10                        ; vbe swi
;mov eax, 1000h                  ; address to store video mode properties
;mov word [eax], 1024            ; width
;mov word [eax + 2], 768         ; height
;mov word [eax + 4], 3072        ; pitch
;mov byte [eax + 6], 24          ; bpp
;mov dword [eax + 7], 0xfd000000 ; physical address of framebuffer


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
mov ecx, 400000h                               ; move 4M of memory
rep movsb

; identity map first 1M of memory

; zero out page directory / page table area
mov al, 0
mov ecx, KERNEL_PAGE_TABLE_BASE - PAGE_DIR_BASE     ; number of bytes to clear
lea edi, [PAGE_DIR_BASE]
rep stosb

; set up directory entry for first 1M of memory
lea ebx, [IDENT_PAGE_TABLE_BASE]
or ebx, 3                                      ; present, rw, kernel memory
mov [PAGE_DIR_BASE], ebx

mov esi, 0                                     ; i = 0
mov ecx, PAGE_SIZE                             ; ecx = 4096

; loop while i < 256 because 256 * PAGE_SIZE = 1M
fill_ident_page_table:
cmp esi, 256                                   ; while (i < 256)
je .done
mov eax, esi                                   ; eax = i
mul ecx                                        ; eax = i * 4096 (addr of current frame)
or eax, 3                                      ; present, rw, kernel memory
mov [IDENT_PAGE_TABLE_BASE + 4 * esi], eax     ; ident_page_table[i] = eax

inc esi                                        ; i++
jmp fill_ident_page_table
.done:

; map 4M of kernel to its virtual address 0xc0000000
; maps virtual addresses 0xc0000000 - 0xc0400000
; to physical addresses  0x00100000 - 0x00500000
;
; again, if kernel grows larger than 4M this will need to be looked at

; point page directory entry which controls our kernel's virtual address to the page table we set up for it
lea ebx, [KERNEL_PAGE_TABLE_BASE]              ; ebx = addr of kernel page table that will map these 4M 
or ebx, 3                                      ; present, rw, kernel memory
mov eax, KERNEL_VIRT_BASE / 400000h            ; eax = index into page directory which controls the page table for kernel's virtual base
mov [PAGE_DIR_BASE + 4 * eax], ebx             ; page_directory[kernel] = ebx

; fill the kernel page table

mov esi, 0                                     ; i = 0
mov ecx, PAGE_SIZE                             ; ecx = 4096

; loop while i < 1024 because 1024 * PAGE_SIZE = 4M
fill_kernel_page_table:
cmp esi, 1024                                  ; while (i < 1024)
je .done
mov eax, esi                                   ; eax = i
mul ecx                                        ; eax = i * 4096 (addr of current frame)
add eax, KERNEL_NEW_BASE                       ; add kernel's physical base to eax
or eax, 3                                      ; present, rw, kernel memory
mov [KERNEL_PAGE_TABLE_BASE + 4 * esi], eax    ; kernel_page_table[i] = eax

inc esi                                        ; i++
jmp fill_kernel_page_table
.done:

; enable paging
lea eax, [PAGE_DIR_BASE]
mov cr3, eax
mov eax, cr0
or eax, 80000000h
mov cr0, eax

; when jumping to kernel, the memory mapping is as follows:
;
; 0x00000000 - 0x00100000 (virtual) is mapped to
; 0x00000000 - 0x00100000 (physical)
;
; 0xc0000000 - 0xc0400000 (virtual) is mapped to
; 0x00100000 - 0x00500000 (physical)
;
; all other addresses are unmapped, and accessing them will cause a page fault.

jmp KERNEL_VIRT_BASE

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

memory_map_error: db 'error detecting memory map', 0
a20_not_enabled: db 'a20 is not enabled!', 0

; constants
MMAP_ADDR              equ  4000h            ; address of memory map
PAGE_DIR_BASE          equ  8000h            ; address of page directory
IDENT_PAGE_TABLE_BASE  equ  9000h            ; address of identity page table
KERNEL_PAGE_TABLE_BASE equ 0a000h            ; address of kernel's page table
KERNEL_LOAD_BASE       equ 10000h            ; address where kernel was loaded by stage1
KERNEL_NEW_BASE        equ 100000h           ; address where linker expects kernel to be loaded by stage2 (1M)

KERNEL_VIRT_BASE       equ 0xc0000000        ; virtual address of start of kernel
PAGE_SIZE              equ  1000h            ; size of page in bytes

; stage1 only loads 1 block (1024 bytes) of stage2 into memory.
; if stage2 grows larger than that, it's no big deal, but stage1
; will need to be modified to load more blocks of stage2.
; this will make it so stage2 won't compile if it's larger than 1024 bytes
; just in case stage2 sneakily grows to that point and not all of it gets loaded

times 1022 - ($ - $$) db 0    ; pad remaining bytes with zeroes
dw 0xcafe                     ; make sure stage 2 doesn't surpass 1024 bytes
