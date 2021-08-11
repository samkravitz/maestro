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

global irq0
irq0:
	cli				; disable interrupts
	mov eax, 32		; hold irq number so dispatcher can identify it
	jmp irq_disp	; jump to common irq dispatcher

global irq1
irq1:
	cli
	mov eax, 33
	jmp irq_disp

global irq2
irq2:
	cli
	mov eax, 34
	jmp irq_disp

global irq3
irq3:
	cli
	mov eax, 35
	jmp irq_disp

global irq4
irq4:
	cli
	mov eax, 36
	jmp irq_disp

global irq5
irq5:
	cli
	mov eax, 37
	jmp irq_disp

global irq6
irq6:
	cli
	mov eax, 38
	jmp irq_disp

global irq7
irq7:
	cli
	mov eax, 39
	jmp irq_disp

global irq8
irq8:
	cli
	mov eax, 40
	jmp irq_disp

global irq9
irq9:
	cli
	mov eax, 41
	jmp irq_disp

global irq10
irq10:
	cli
	mov eax, 42
	jmp irq_disp

global irq11
irq11:
	cli
	mov eax, 43
	jmp irq_disp

global irq12
irq12:
	cli
	mov eax, 44
	jmp irq_disp

global irq13
irq13:
	cli
	mov eax, 45
	jmp irq_disp

global irq14
irq14:
	cli
	mov eax, 46
	jmp irq_disp

global irq15
irq15:
	cli
	mov eax, 47
	jmp irq_disp

; irq dispatcher
irq_disp:
	pusha		; save state pre-interrupt
	push eax	; pass irq number as parameter that was saved earlier 
	call irq	; call the registered irq handler
	pop eax		; clean up stack
	popa		; restore state
	sti			; restore interrupts
	iret 		; return from interrupt
