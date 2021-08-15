; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: boot.s
; DATE: July 26, 2021
; DESCRIPTION: The first code executed when the computer turns on -
; 	sets up multiboot data, initializes the GDT, and calls kmain()
;
; this bootloader was graciously adapted from
; http://3zanders.co.uk/2017/10/13/writing-a-bootloader/
; and
; https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf
[bits 32]

; constants required for multiboot header
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
 
; set up multiboot header
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

extern kmain
section .text
global boot:
boot:
	lgdt [gdt_descriptor]	; load gdt into gdtr
	mov ax, 0x10      		; 0x10 is the offset in the gdt to data segment
	mov ds, ax        		; reload remainder of data segment registers
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x8:ljmp			; perform long jump to activate new segment registers

	ljmp:					; dummy label to act as landing point for long jump
	mov esp, kstack_top		; load esp with kernel stack
	call kmain 
	
	loop:
	hlt
	jmp loop

; initialize gdt
section .data
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

; initialize kernel stack
section .bss
align 16
kstack_bottom:
resb 16384 ; 16 KiB
kstack_top:
