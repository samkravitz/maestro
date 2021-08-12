; This is a short program I wrote to explore what the value of esp is when it is pushed during a pusha instruction.
; example output:
; stack ptr at beginning is:   0xbfc9d770
; stack ptr at end is:         0xbfc9d750
; stack ptr saved in pusha is: 0xbfc9d770
[bits 32]

global stack
global main
extern printf

section .text
main:
	push ebp
	mov ebp, esp
	call stack				; get it? call stack?
	pop ebp
	mov eax, 0
	ret

stack:
	push ebp
	mov ebp, esp
	mov eax, .stackbeg
	push esp
	push eax
	call printf
	add esp, 8
	pusha
	push esp
	mov eax, .stackend
	push eax
	call printf
	add esp, 8
	mov eax, esp
	add eax, 12				; esp is the 3rd from last reg pushed in a push instruction. 3 * 4 bytes per reg = 12
	mov eax, [eax]
	mov ebx, .stackpush
	push eax
	push ebx
	call printf
	add esp, 8
	popa
	mov eax, 0
	pop ebp
	ret

section .data

.stackbeg:
	db `stack ptr at beginning is:   0x%x\n`, 0

.stackend:
	db `stack ptr at end is:         0x%x\n`, 0

.stackpush:
	db `stack ptr saved in pusha is: 0x%x\n`, 0