; maestro
; License: GPLv2
; See LICENSE.txt for full license text
; Author: Sam Kravitz
;
; FILE: start.s
; DATE: March 7th, 2022
; DESCRIPTION: The entry point to the kernel after the bootloader loads it -
;     sets up a stack, GDT, and calls kmain

[bits 32]

global entry
global kpage_dir
global kpage_table
global ident_page_table
global set_task
global fb_page_table
global stack_trace
global kstack_top

extern kmain
extern vkprintf

section .entry
entry:
jmp .start

section .text
.start:
lgdt [gdt_descriptor]      ; load gdt into gdtr
mov ax, 10h                ; 10h is the offset in the gdt to data segment
mov ds, ax                 ; reload remainder of data segment registers
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

; set up task segment in gdt like this:
;	Base = &tss
;	Limit = sizeof(tss)
;	Access Byte = 89h
;	Flags = 0
mov eax, tss_end - tss     ; eax = sizeof(tss)
mov word [gdt_ts], ax

mov eax, tss               ; eax = &tss
mov word [gdt_ts + 2], ax  ; store [&tss & 0xffff] to gdt
shr eax, 16                ; eax >>= 16
mov byte [gdt_ts + 4], al  ; store [&tss >> 16 & 0xff] to gdt
shr eax, 8                 ; eax >>= 8
mov byte [gdt_ts + 7], al  ; store [&tss >> 24 & 0xff] to gdt

mov ax, 28h                ; 28h is offset into gdt to task segment
ltr ax                     ; load task segment to task register

mov esp, kstack_top        ; load esp with kernel stack
xor ebp, ebp               ; ebp = 0 (to calculate final stack frame)

call kmain

jmp $                      ; kernel should never return

; void stack_trace(u32 start_ebp)
; if start_ebp is 0, traces from caller's frame
; if start_ebp is non-zero, traces from that ebp
stack_trace:
	push ebp
	mov ebp, esp
	pusha

	mov ebx, [ebp + 8]     ; ebx = first parameter (start_ebp)
	cmp ebx, 0
	jne .start             ; if start_ebp != 0, use it
	mov ebx, [ebp]         ; else use caller's ebp (saved ebp points to caller's frame)

	.start:
	xor eax, eax           ; eax = 0 (frame counter)

	.trace:
	cmp ebx, 0             ; (if ebx == NULL) return;
	je .done

	mov edx, [ebx + 4]     ; edx = current stack frame's ip
	mov ebx, [ebx]         ; ebx = frame->next

	push edx               ; print out current frame's ip
	push eax
	push stacktrace_msg
	call vkprintf
	add esp, 4
	pop eax
	pop edx

	inc eax                ; i++
	jmp .trace

	.done:
	popa
	pop ebp
	ret

; sets a new task's kernel stack pointer in the esp0 field in tss
; cdecl - void set_task(u32 esp)
set_task:
	mov eax, [esp + 4]
	mov [tss.esp0], eax
	ret

; initialize gdt
section .data
gdt:
gdt_null:                  ; null descriptor
	dd 0                   ; 4 bytes of 0
	dd 0                   ; 4 bytes of 0

; kernel segments
gdt_kcode:                 ; kernel mode code segment
	dw 0ffffh              ; limit (bits 0-15)
	dw 0                   ; base  (bits 0-15)
	db 0                   ; base  (bits 16-23)
	db 10011010b           ; flags
	db 11001111b           ; flags cont., limit (bits 16-19)
	db 0                   ; base (bits 24-31)

gdt_kdata:                 ; kernel mode data segment
	dw 0ffffh              ; limit (bits 0-15)
	dw 0                   ; base (bits 0-15)
	db 0                   ; base (bits 16-23)
	db 10010010b           ; flags
	db 11001111b           ; flags cont., limit (bits 16-19)
	db 0                   ; base (bits 24-31)

; user segments
gdt_ucode:                 ; user mode code segment
	dw 0ffffh              ; limit (bits 0-15)
	dw 0                   ; base  (bits 0-15)
	db 0                   ; base  (bits 16-23)
	db 11111010b           ; flags
	db 11001111b           ; flags cont., limit (bits 16-19)
	db 0                   ; base (bits 24-31)

gdt_udata:                 ; user mode data segment
	dw 0ffffh              ; limit (bits 0-15)
	dw 0                   ; base (bits 0-15)
	db 0                   ; base (bits 16-23)
	db 11110010b           ; flags
	db 11001111b           ; flags cont., limit (bits 16-19)
	db 0                   ; base (bits 24-31)

; task segment
gdt_ts:
	dw 0                   ; limit (bits 0-15)
	dw 0                   ; base (bits 0-15)
	db 0                   ; base (bits 16-23)
	db 10001001b           ; flags (access byte)
	db 0                   ; flags cont., limit (bits 16-19)
	db 0                   ; base (bits 24-31)
gdt_end:

tss:
.prev_tss: dd 0            ; selector of previous task's tss
.esp0:     dd kstack_top   ; ring0 stack pointer
.ss0:      dd 10h          ; ring0 stack segment
.esp1:     dd 0            ; ring1 stack pointer
.ss1:      dd 0            ; ring1 stack segment
.esp2:     dd 0            ; ring2 stack pointer
.ss2:      dd 0            ; ring2 stack segment
.cr3:      dd 0
.eflags:   dd 0
.eax:      dd 0
.ecx:      dd 0
.edx:      dd 0
.ebx:      dd 0
.esp:      dd 0
.ebp:      dd 0
.esi:      dd 0
.edi:      dd 0
.es:       dd 13h
.cs:       dd 0xb
.ss:       dd 13h
.ds:       dd 13h
.fs:       dd 13h
.gs:       dd 13h
.ldt:      dd 0
.trap:     dw 0
.iomap:    dw 0
tss_end:

; 6 byte value to be stored in gdtr
gdt_descriptor:
dw gdt_end - gdt - 1       ; size of gdt minus 1
dd gdt                     ; starting address of GDT

stacktrace_msg:
	db `callstack[%d]: 0x%x\n`,0

section .bss
; kernel stack
align 16
kstack_bottom:
resb 16384				   ; reserve 16K for kernel stack
kstack_top:
