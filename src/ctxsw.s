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
    mov ebp, [esp + 4]
    popf
    add esp, 4
    ret
