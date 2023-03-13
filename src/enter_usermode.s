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

; cdecl - void enter_usermode(void *_start)
enter_usermode:
	mov ax, 20h | 3    ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax
 
	; set up the stack frame iret expects
    mov ecx, [esp + 4]     ; ecx = (void *) _start
    mov eax, USTACK_BOTTOM ; eax = ustack_bottom
	push 20h | 3           ; user mode data selector
	push eax               ; current esp
	pushf                  ; eflags
	push 18h | 3           ; user mode code selector
	push ecx               ; return address (start of user process)
	iret

USTACK_BOTTOM equ 0xc0000000 ; address of bottom of user stack
