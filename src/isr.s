; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: isr.s
; DATE: August 1st, 2021
; DESCRIPTION: Dispatchers for interrupt service routines
[bits 32]

; isr handler
extern isr

; note - isr8, 10-14 push error code automaticcaly by cpu

global isr0
isr0:
	cli				; disable interrupts
	push 0			; push dummy error code
	push 0			; push interrupt number
	jmp isr_disp	; jump to isr dispatcher

global isr1
isr1:
	cli
	push 0
	push 1
	jmp isr_disp

global isr2
isr2:
	cli
	push 0
	push 2
	jmp isr_disp

global isr3
isr3:
	cli
	push 0
	push 3
	jmp isr_disp

global isr4
isr4:
	cli
	push 0
	push 4
	jmp isr_disp

global isr5
isr5:
	cli
	push 0
	push 5
	jmp isr_disp

global isr6
isr6:
	cli
	push 0
	push 6
	jmp isr_disp

global isr7
isr7:
	cli
	push 0
	push 7
	jmp isr_disp

global isr8
isr8:
	cli
	push 8
	jmp isr_disp

global isr9
isr9:
	cli
	push 0
	push 9
	jmp isr_disp

global isr10
isr10:
	cli
	push 10
	jmp isr_disp

global isr11
isr11:
	cli
	push 11
	jmp isr_disp

global isr12
isr12:
	cli
	push 12
	jmp isr_disp

global isr13
isr13:
	cli
	push 13
	jmp isr_disp

global isr14
isr14:
	cli
	push 14
	jmp isr_disp

global isr15
isr15:
	cli
	push 0
	push 15
	jmp isr_disp

global isr16
isr16:
	cli
	push 0
	push 16
	jmp isr_disp

global isr17
isr17:
	cli
	push 0
	push 17
	jmp isr_disp

global isr18
isr18:
	cli
	push 0
	push 18
	jmp isr_disp

global isr19
isr19:
	cli
	push 0
	push 19
	jmp isr_disp

global isr20
isr20:
	cli
	push 0
	push 20
	jmp isr_disp

global isr21
isr21:
	cli
	push 0
	push 21
	jmp isr_disp

global isr22
isr22:
	cli
	push 0
	push 22
	jmp isr_disp

global isr23
isr23:
	cli
	push 0
	push 23
	jmp isr_disp

global isr24
isr24:
	cli
	push 0
	push 24
	jmp isr_disp

global isr25
isr25:
	cli
	push 0
	push 25
	jmp isr_disp

global isr26
isr26:
	cli
	push 0
	push 26
	jmp isr_disp

global isr27
isr27:
	cli
	push 0
	push 27
	jmp isr_disp

global isr28
isr28:
	cli
	push 0
	push 28
	jmp isr_disp

global isr29
isr29:
	cli
	push 0
	push 29
	jmp isr_disp

global isr30
isr30:
	cli
	push 0
	push 30
	jmp isr_disp

global isr31
isr31:
	cli
	push 0
	push 31
	jmp isr_disp

; isr dispatcher
isr_disp:
	pusha			; push all registers for state structure
   	call isr		; call isr dispatcher
   	popa
   	add esp, 8		; clean up stack
   	sti				; enable interrupts
   	iret
