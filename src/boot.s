; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: boot.s
; DATE: July 26, 2021
; DESCRIPTION: The first code executed when the computer turns on -
; 	switches to protected mode, initializes the GDT, and calls kmain()
;
; this bootloader was graciously adapted from
; http://3zanders.co.uk/2017/10/13/writing-a-bootloader/
; and
; https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf

section .boot
[bits 16]
global boot
extern kmain
boot:
	; enable VGA text mode
	mov ax, 0x3
	int 0x10

	; load kernel from disk to run code beyond bootlaoder
	mov [disk], dl	; store disk sector into reserved memory
	mov ah, 0x2		; read sectors
	mov al, 32		; sectors to read
	mov ch, 0		; cylinder idx
	mov dh, 0		; head idx
	mov cl, 2		; sector idx
	mov dl, [disk]	; disk idx
	mov bx, main	; target pointer
	int 0x13
	cli
	lgdt [gdt_descriptor]

	; switch to 32 bit protected mode
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp CODE_SEG:main

; initialize GDT
gdt:
gdt_null:      ; null descriptor
	dd 0x0       ; 4 bytes of 0
	dd 0x0       ; 4 bytes of 0
gdt_code:      ; code segment descriptor
	dw 0xffff    ; limit (bits 0-15)
	dw 0x0       ; base  (bits 0-15)
	db 0x0       ; base  (bits 16-23)
	db 10011010b ; flags
	db 11001111b ; flags cont., limit (bits 16-19)
	db 0x0       ; base (bits 24-31)
gdt_data:      ; the data segment descriptor
	dw 0xffff    ; limit (bits 0-15)
	dw 0x0       ; base (bits 0-15)
	db 0x0       ; base (bits 16-23)
	db 10010010b ; flags
	db 11001111b ; flags cont., limit (bits 16-19)
	db 0x0       ; base (bits 24-31)
gdt_end:

; 6 byte value to be stored in gdtr
gdt_descriptor:
dw gdt_end - gdt - 1 ; size of gdt minus 1
dd gdt               ; starting address of GDT

CODE_SEG equ gdt_code - gdt
DATA_SEG equ gdt_data - gdt

; reserve a byte to store boot drive number to use later later
disk:
	db 0

; pad the remainder of the 512 byte bootsector with 0
; and remember to end with 0x55aa (little endian)
times 510 - ($-$$) db 0
dw 0xaa55

[bits 32]
main:
	mov esp, kstack_top
	call kmain
	cli
	hlt

; initialize kernel stack
section .bss
align 4
kstack_bottom: equ $
	resb 16384 ; 16 KB
kstack_top:
