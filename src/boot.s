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

%define virt_to_phys(addr) (addr - 0xc0000000)

; virtual address of kernel start and end
extern start
extern end

; reserve memory for page table and page directory
section .bss
align 4096
page_directory:
resb 4096
page_table:
resb 4096

extern kmain
section .text
global boot:
boot:
	; identity map kernel to 0xc0000000
	mov edi, virt_to_phys(page_table)
	mov esi, 0
	mov ecx, 1023

    .1:
    cmp esi, start
    jl .2
    cmp esi, virt_to_phys(end)
    jge .3
	; mark page as present & writable
    mov edx, esi
    or edx, 0x3
    mov [edi], edx

    .2:
    add esi, 4096
    add edi, 4
    loop .1

    .3:
	; move VGA video memory (0xb8000) to 1024th page
    mov dword [virt_to_phys(page_table) + 1023 * 4], 0xb8003
	; use same page table for page directory entry 0 and page directory entry 768
	mov dword [virt_to_phys(page_directory) + 0], virt_to_phys(page_table) + 0x003
	mov dword [virt_to_phys(page_directory) + 768 * 4], virt_to_phys(page_table) + 0x003

	; set cr3 to the address of the page directory
	mov ecx, virt_to_phys(page_directory)
	mov cr3, ecx

	; enable paging and the write-protect bit
	mov ecx, cr0
	or ecx, 0x80010000
	mov cr0, ecx

	; jump to higher half
	lea ecx, .4
	jmp ecx

	.4:
	; unmap the identity mapping as it is now unnecessary
	mov dword [page_directory + 0], 0

	; reload cr3 to force a TLB flush so the changes take effect
	mov ecx, cr3
	mov cr3, ecx

	mov [mboot_info], ebx		; store multiboot info for later
	lgdt [gdt_descriptor]		; load gdt into gdtr
	mov ax, 0x10				; 0x10 is the offset in the gdt to data segment
	mov ds, ax					; reload remainder of data segment registers
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x8:main			; perform long jump to activate new segment registers

	main:					; dummy label to act as landing point for long jump
	mov esp, kstack_top		; load esp with kernel stack
	call kmain 
	
	jmp $$					; kernel should never return

; initialize gdt
section .data
gdt:
gdt_null:			; null descriptor
	dd 0x0			; 4 bytes of 0
	dd 0x0       	; 4 bytes of 0

gdt_code:			; code segment descriptor
	dw 0xffff		; limit (bits 0-15)
	dw 0x0			; base  (bits 0-15)
	db 0x0			; base  (bits 16-23)
	db 10011010b	; flags
	db 11001111b	; flags cont., limit (bits 16-19)
	db 0x0			; base (bits 24-31)

gdt_data:			; data segment descriptor
	dw 0xffff		; limit (bits 0-15)
	dw 0x0			; base (bits 0-15)
	db 0x0			; base (bits 16-23)
	db 10010010b	; flags
	db 11001111b	; flags cont., limit (bits 16-19)
	db 0x0			; base (bits 24-31)
gdt_end:

; 6 byte value to be stored in gdtr
gdt_descriptor:
dw gdt_end - gdt - 1	; size of gdt minus 1
dd gdt					; starting address of GDT

global mboot_info:
mboot_info:
	dd 0

; initialize kernel stack
section .bss
align 16
kstack_bottom:
resb 16384				; 16K
kstack_top:
