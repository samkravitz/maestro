;  maestro
;  License: GPLv2
;  See LICENSE.txt for full license text
;  Author: Sam Kravitz
; 
;  FILE: ctsw.s
;  DATE: August 10th, 2021
;  DESCRIPTION: switch to a new process
[bits 32]

; cdecl - void ctxsw(void *oldstk, void *newstk)
global ctxsw:
ctxsw:
	mov eax, [esp + 4]		; old process
	mov ecx, [esp + 8]		; new process
	pushf				; save callee-saved registers
	push ebp
	push ebx
	push esi
	push edi
	mov [eax], esp      		; save old stack
	mov esp, ecx
	pop edi				; restore callee-saved registers of new process
	pop esi
	pop ebx
	pop ebp
	popf
	ret
