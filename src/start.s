; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: start.s
; DATE: March 7th, 2022
; DESCRIPTION: The entry point to the kernel after the bootloader loads it -
;     sets up a stack, GDT, and calls kmain

[bits 32]

global entry
global kpage_dir
global kpage_table
global ident_page_table

extern clear
extern kmain

section .entry
entry:
jmp .start

section .text
.start:
lgdt [gdt_descriptor]      ; load gdt into gdtr
mov ax, 10h                ; 10h is the offset in the gdt to data segment
mov ds, ax                 ; reload remainder of data segment registers
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

mov esp, kstack_top        ; load esp with kernel stack

call clear                 ; clear screen
call kmain

jmp $                      ; kernel should never return

; initialize gdt
section .data
gdt:
gdt_null:                  ; null descriptor
	dd 0                   ; 4 bytes of 0
	dd 0                   ; 4 bytes of 0

; kernel segments
gdt_kcode:                 ; kernel mode code segment
	dw 0ffffh              ; limit (bits 0-15)
	dw 0                   ; base  (bits 0-15)
	db 0                   ; base  (bits 16-23)
	db 10011010b           ; flags
	db 11001111b           ; flags cont., limit (bits 16-19)
	db 0                   ; base (bits 24-31)

gdt_kdata:                 ; kernel mode data segment
	dw 0ffffh              ; limit (bits 0-15)
	dw 0                   ; base (bits 0-15)
	db 0                   ; base (bits 16-23)
	db 10010010b           ; flags
	db 11001111b           ; flags cont., limit (bits 16-19)
	db 0                   ; base (bits 24-31)

; user segments
gdt_ucode:                 ; user mode code segment
	dw 0ffffh              ; limit (bits 0-15)
	dw 0                   ; base  (bits 0-15)
	db 0                   ; base  (bits 16-23)
	db 11111010b           ; flags
	db 11001111b           ; flags cont., limit (bits 16-19)
	db 0                   ; base (bits 24-31)

gdt_udata:                 ; user mode data segment
	dw 0ffffh              ; limit (bits 0-15)
	dw 0                   ; base (bits 0-15)
	db 0                   ; base (bits 16-23)
	db 11110010b           ; flags
	db 11001111b           ; flags cont., limit (bits 16-19)
	db 0                   ; base (bits 24-31)

gdt_end:

; 6 byte value to be stored in gdtr
gdt_descriptor:
dw gdt_end - gdt - 1       ; size of gdt minus 1
dd gdt                     ; starting address of GDT

section .bss
; kernel stack
align 16
kstack_bottom:
resb 16384				   ; reserve 16K for kernel stack
kstack_top:

; kernel page directory
align 0x1000               ; must be page aligned
kpage_dir:
resb 1024 * 4              ; page directory is 1024 4 byte entries

; page table that maps first 4M of kernel
align 0x1000               ; must be page aligned
kpage_table:
resb 1024 * 4              ; page table is 1024 4 byte entries

; page table that identity maps first 1M of physical memory
align 0x1000               ; must be page aligned
ident_page_table:
resb 1024 * 4              ; page table is 1024 4 byte entries
