; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: ctsw.s
; DATE: August 10th, 2021
; DESCRIPTION: switch to a new process
; RESOURCE: http://samwho.dev/blog/context-switching-on-x86
[bits 32]

	global ctxsw
	extern set_task

; cdecl - void ctxsw(void *oldstk, void *newstk)
ctxsw:
	mov eax, [esp + 4]      ; old process
	mov ecx, [esp + 8]      ; new process

	; save callee-saved registers
	push ebp
	push ebx
	push esi
	push edi

	; switch stacks
	mov [eax], esp          ; save old stack
	mov esp, [ecx]          ; move new stack into esp

	; set new process's kernel stack in tss so it can be loaded
	; when the process is preempted
	push ecx
	call set_task
	add esp, 4

	; restore callee-saved registers of new process
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
