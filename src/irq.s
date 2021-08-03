; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: irq.s
; DATE: August 2nd, 2021
; DESCRIPTION: Dispatchers for hardware interrupts
[bits 32]

extern _irq

global irq0
irq0:
    cli				; disable interrupts
	mov eax, 32		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq1
irq1:
    cli				; disable interrupts
	mov eax, 33		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq2
irq2:
    cli				; disable interrupts
    mov eax, 34		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq3
irq3:
    cli				; disable interrupts
    mov eax, 35		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq4
irq4:
    cli				; disable interrupts
    mov eax, 36		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq5
irq5:
    cli				; disable interrupts
    mov eax, 37		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq6
irq6:
    cli				; disable interrupts
    mov eax, 38		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq7
irq7:
    cli				; disable interrupts
    mov eax, 39		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq8
irq8:
    cli				; disable interrupts
    mov eax, 40		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq9
irq9:
    cli				; disable interrupts
    mov eax, 41		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq10
irq10:
    cli				; disable interrupts
    mov eax, 42		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq11
irq11:
    cli				; disable interrupts
    mov eax, 43		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq12
irq12:
    cli				; disable interrupts
    mov eax, 44		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq13
irq13:
    cli				; disable interrupts
    mov eax, 45		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq14
irq14:
    cli				; disable interrupts
    mov eax, 46		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

global irq15
irq15:
    cli				; disable interrupts
    mov eax, 47		; hold irq number so dispatcher can identify it
    jmp irq_disp	; jump to common irq dispatcher

irq_disp:
	pusha       ; save state pre-interrupt
	push eax	; pass irq number as parameter that was saved earlier 
   	call _irq   ; call the registered irq handler
   	pop eax		; clean up stack
   	popa        ; restore state
   	sti			; restore interrupts
   	iret		; return from interrupt