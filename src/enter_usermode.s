;  maestro
;  License: GPLv2
;  See LICENSE.txt for full license text
;  Author: Sam Kravitz
; 
;  FILE: enter_usermode.s
;  DATE: March 28th, 2022
;  DESCRIPTION: trampolines the current running process to user mode
[bits 32]

	global enter_usermode

enter_usermode:
	mov ax, 20h | 3    ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax
 
	; set up the stack frame iret expects
	mov eax, esp
	push 20h | 3       ; user mode data selector
	push eax           ; current esp
	pushf              ; eflags
	push 18h | 3       ; user mode code selector
	lea eax, [after]
	push eax
	iret

	after:
		ret
