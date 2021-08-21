[bits 32]

global ctxsw:
ctxsw:
	push ebp
	mov ebp, esp
	pushf
	pusha
	mov eax, [ebp + 8]
	mov [eax], esp
	mov eax, [ebp + 12]
	mov esp, [eax]
	popa
	popf
	mov esp, ebp
	pop ebp
	ret
