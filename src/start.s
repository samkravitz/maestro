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

section .entry
global start
start:
jmp main

section .text
main:
lgdt [gdt_descriptor]      ; load gdt into gdtr
mov ax, 10h                ; 10h is the offset in the gdt to data segment
mov ds, ax                 ; reload remainder of data segment registers
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

mov esp, kstack_top        ; load esp with kernel stack

extern clear
call clear

extern kmain
call kmain 

jmp $              ; kernel should never return

; initialize gdt
section .data
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

global mboot_info:
mboot_info:
	dd 0

; initialize kernel stack
section .bss
align 16
kstack_bottom:
resb 16384				; 16K
kstack_top:
