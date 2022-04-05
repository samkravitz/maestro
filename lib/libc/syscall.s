; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: libc/syscall.s
; DATE: April 5th, 2022
; DESCRIPTION: C library syscall wrappers, called from libc/syscall.c
[bits 32]

	global syscall0
	global syscall1
	global syscall2
	global syscall3

	section .text

; syscall with 0 arguments
syscall0:
	push ebp
	mov ebp, esp
	mov eax, [ebp + 8]     ; sysno
	int 48                 ; syscall
	pop ebp
	ret

; syscall with 1 argument
syscall1:
	push ebp
	mov ebp, esp
	push ebx
	mov eax, [ebp + 8]     ; sysno
	mov ebx, [ebp + 12]    ; arg1
	int 48                 ; syscall
	pop ebx
	pop ebp
	ret

; syscall with 2 arguments
syscall2:
	push ebp
	mov ebp, esp
	push ebx
	push ecx
	mov eax, [ebp + 8]     ; sysno
	mov ebx, [ebp + 12]    ; arg1
	mov ecx, [ebp + 16]    ; arg2
	int 48                 ; syscall
	pop ecx
	pop ebx
	pop ebp
	ret

; syscall with 3 arguments
syscall3:
	push ebp
	mov ebp, esp
	push ebx
	push ecx
	push edx
	mov eax, [ebp + 8]     ; sysno
	mov ebx, [ebp + 12]    ; arg1
	mov ecx, [ebp + 16]    ; arg2
	mov edx, [ebp + 20]    ; arg3
	int 48                 ; syscall
	pop edx
	pop ecx
	pop ebx
	pop ebp
	ret
