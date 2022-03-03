; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: stage2.s
; DATE: March 3rd, 2022
; DESCRIPTION: stage 2 bootloader

[bits 16]
org 0x5000    ; we are expecting stage1 to load us to memory address 0x5000

mov si, hello
call puts
jmp $

hello: db 'hello from stage2!', 0

; prints a string to the screen
; si - pointer to string to be printed
puts:
    lodsb
	or al, al		; test for NULL termination
	jz .done 
	mov ah, 0xe 
	int 10h
	jmp puts
.done:
	ret
