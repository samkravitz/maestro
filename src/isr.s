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

global isr0
isr0:
	cli
	mov eax, esp
	push eax
	push $0
	jmp isr_disp

global isr1
isr1:
	cli
	mov eax, esp
	push eax
	push $1
	jmp isr_disp

global isr2
isr2:
	cli
	mov eax, esp
	push eax
	push $2
	jmp isr_disp

global isr3
isr3:
	cli
	mov eax, esp
	push eax
	push $3
	jmp isr_disp

global isr4
isr4:
	cli
	mov eax, esp
	push eax
	push $4
	jmp isr_disp

global isr5
isr5:
	cli
	mov eax, esp
	push eax
	push $5
	jmp isr_disp

global isr6
isr6:
	cli
	mov eax, esp
	push eax
	push $6
	jmp isr_disp

global isr7
isr7:
	cli
	mov eax, esp
	push eax
	push $7
	jmp isr_disp

global isr8
isr8:
	cli
	mov eax, esp
	push eax
	push $8
	jmp isr_disp

global isr9
isr9:
	cli
	mov eax, esp
	push eax
	push $9
	jmp isr_disp

global isr10
isr10:
	cli
	mov eax, esp
	push eax
	push $10
	jmp isr_disp

global isr11
isr11:
	cli
	mov eax, esp
	push eax
	push $11
	jmp isr_disp

global isr12
isr12:
	cli
	mov eax, esp
	push eax
	push $12
	jmp isr_disp

global isr13
isr13:
	cli
	mov eax, esp
	push eax
	push $13
	jmp isr_disp

global isr14
isr14:
	cli
	mov eax, esp
	push eax
	push $14
	jmp isr_disp

global isr15
isr15:
	cli
	mov eax, esp
	push eax
	push $15
	jmp isr_disp

global isr16
isr16:
	cli
	mov eax, esp
	push eax
	push $16
	jmp isr_disp

global isr17
isr17:
	cli
	mov eax, esp
	push eax
	push $17
	jmp isr_disp

global isr18
isr18:
	cli
	mov eax, esp
	push eax
	push $18
	jmp isr_disp

global isr19
isr19:
	cli
	mov eax, esp
	push eax
	push $19
	jmp isr_disp

global isr20
isr20:
	cli
	mov eax, esp
	push eax
	push $20
	jmp isr_disp

global isr21
isr21:
	cli
	mov eax, esp
	push eax
	push $21
	jmp isr_disp

global isr22
isr22:
	cli
	mov eax, esp
	push eax
	push $22
	jmp isr_disp

global isr23
isr23:
	cli
	mov eax, esp
	push eax
	push $23
	jmp isr_disp

global isr24
isr24:
	cli
	mov eax, esp
	push eax
	push $24
	jmp isr_disp

global isr25
isr25:
	cli
	mov eax, esp
	push eax
	push $25
	jmp isr_disp

global isr26
isr26:
	cli
	mov eax, esp
	push eax
	push $26
	jmp isr_disp

global isr27
isr27:
	cli
	mov eax, esp
	push eax
	push $27
	jmp isr_disp

global isr28
isr28:
	cli
	mov eax, esp
	push eax
	push $28
	jmp isr_disp

global isr29
isr29:
	cli
	mov eax, esp
	push eax
	push $29
	jmp isr_disp

global isr30
isr30:
	cli
	mov eax, esp
	push eax
	push $30
	jmp isr_disp

global isr31
isr31:
	cli
	mov eax, esp
	push eax
	push $31
	jmp isr_disp

; isr dispatcher
isr_disp:
	pusha          ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
   	call isr
   	popa           ; Pops edi,esi,ebp...
   	add esp, 8     ; Cleans up the pushed error code and pushed ISR number
   	sti
   	iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP