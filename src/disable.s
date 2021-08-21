;  maestro
;  License: GPLv2
;  See LICENSE.txt for full license text
;  Author: Sam Kravitz
; 
;  FILE: disable.s
;  DATE: August 20th, 2021
;  DESCRIPTION: locally disable and restore interrupts
[bits 32]

; cdecl - void disable(u32 *flags)
; populates flags with the value of the IF register in eflags
global disable:
disable:
	push ebp					; set up frame
	mov ebp, esp
	pushf						; push flags on stack
	pop eax						; pop flags from stack into eax
	cli							; disable interrupts
	and eax, 0x202				; clear all bits but  IF flag (0x200) and reserved flag (0x2)
	mov ecx, [ebp + 8]			; ecx <- ptr to flags to save
	mov [ecx], eax				; store saved flags into pointer
	pop ebp
	ret

; cdecl - void restore(u32 flags)
; restores IF in eflags to the passed value
global restore:
restore:
	cli							; disable interrupts
	push ebp					; set up stack frame
	mov ebp, esp
	mov eax, [ebp + 8]			; eax <- saved flags argument
	pushf						; push eflags on stack
	pop ecx						; pop flags into ecx
	or eax, ecx					; restore bits saved in disable() (0x200 and 0x2)
	push eax					; push eax (containing restored flags) onto stack
	popf						; pop stack into eflags
	pop ebp
	ret



