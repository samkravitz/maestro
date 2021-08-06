[bits 32]

extern cpd

; page directory switch
; @param 1 - struct pagedir * - pointer of pagedir which will become current
; @param2  - u32 - physical address of the pagedir that will be placed in cr0
global pdsw
pdsw:
	push ebp			; save base ptr
	mov ebp, esp		; move top of stack to base ptr
	cli					; disable interrupts
	mov eax, [ebp + 8]	; mov struct pagedir into eax
	mov [cpd], eax		; make struct pagedir current pag
	mov eax, cr0		; store cr0 in eax to operate on
	or  eax, 0x80000000	; enable paging
	mov cr0, eax		; store modified cr0 back in cr0
	mov eax, [ebp + 12]	; move 2nd arugment (physaddr) into eax
	mov cr3, eax		; move physaddr into cr3
	pop ebp				; clean up stack
	sti					; re-enable interrupts
	ret
