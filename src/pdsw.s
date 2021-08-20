; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: pdsw.s
; DATE: August 8th, 2021
; DESCRIPTION: pdsw
[bits 32]

; page directory switch
; @param struct pagedir * - pointer of pagedir which will become current
global pdsw
pdsw:
	push ebp			; save base ptr
	mov ebp, esp		; move top of stack to base ptr
	;cli					; disable interrupts
	mov eax, [ebp + 8]	; move 2nd arugment (physaddr) into eax
	mov cr3, eax		; move physaddr into cr3
	mov eax, cr0		; store cr0 in eax to operate on
	or  eax, 0x80000000	; enable paging
	mov cr0, eax		; store modified cr0 back in cr0
	pop ebp				; clean up stack
	;sti					; re-enable interrupts
	ret
