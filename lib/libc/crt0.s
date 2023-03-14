; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: libc/crt0.s
; DATE: April 27th, 2022
; DESCRIPTION: defines _start symbol for userspace processes
[bits 32]

	global _start
	extern main
	extern exit

	section .text

_start:
	xor ebx, ebx
	push ebx
	mov ebp, esp

	call main
    push eax
    call exit
