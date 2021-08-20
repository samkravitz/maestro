; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: irq.s
; DATE: August 2nd, 2021
; DESCRIPTION: Dispatchers for hardware interrupts
[bits 32]

extern irq
extern currframe
extern currframeptr

global irq0
irq0:
	mov eax, 32		; hold irq number so dispatcher can identify it
	jmp irq_disp	; jump to common irq dispatcher

global irq1
irq1:
	mov eax, 33
	jmp irq_disp

global irq2
irq2:
	mov eax, 34
	jmp irq_disp

global irq3
irq3:
	mov eax, 35
	jmp irq_disp

global irq4
irq4:
	mov eax, 36
	jmp irq_disp

global irq5
irq5:
	mov eax, 37
	jmp irq_disp

global irq6
irq6:
	mov eax, 38
	jmp irq_disp

global irq7
irq7:
	mov eax, 39
	jmp irq_disp

global irq8
irq8:
	mov eax, 40
	jmp irq_disp

global irq9
irq9:
	mov eax, 41
	jmp irq_disp

global irq10
irq10:
	mov eax, 42
	jmp irq_disp

global irq11
irq11:
	mov eax, 43
	jmp irq_disp

global irq12
irq12:
	mov eax, 44
	jmp irq_disp

global irq13
irq13:
	mov eax, 45
	jmp irq_disp

global irq14
irq14:
	mov eax, 46
	jmp irq_disp

global irq15
irq15:
	mov eax, 47
	jmp irq_disp

; irq dispatcher
irq_disp:
	pusha		; save state pre-interrupt
	push eax	; pass irq number as parameter that was saved earlier 
	call irq	; call the registered irq handler
	pop eax		; clean up stack
	popa		; restore state
	iret 		; return from interrupt

	push ebp
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
	mov eax, 32
	push eax	; pass irq number as parameter that was saved earlier 
	call irq	; call the registered irq handler
	pop eax		; clean up stack
	
	mov eax, currframe
	mov eax, [eax]
	mov esp, eax
	pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    pop ebp
    
    ;add esp, 4

	; mov eax, [esp]
	; push eax
	; mov eax, da
	; push eax
	; extern koutf
	; call koutf
	; add esp, 8


	; loop:
	; jmp loop
	iret

da:
	db `eip: %x\n`, 0




; ctxsw:
; 	push ebp
; 	mov ebp, esp
; 	push ebp
;     push edi
;     push esi

;     push edx
;     push ecx
;     push ebx
;     push eax

; 	push esp
; 	mov eax, currframe
; 	mov esp, eax
; 	pop eax
;     pop ebx
;     pop ecx
;     pop edx
; 	pop esi
;     pop edi
;     pop ebp

; 	add esp, 4
; 	ret







; pusha		; save state pre-interrupt
; push eax	; pass irq number as parameter that was saved earlier 
; call irq	; call the registered irq handler
; pop eax		; clean up stack
; popa		; restore state
; iret 		; return from interrupt