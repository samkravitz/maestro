[bits 32]

extern printB
extern koutf

global ctxsw:
ctxsw:
    cli
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
    sti
    ret
    ;add esp, 12
    ;push printB
    ;ret

    ; push ebp
    ; mov ebp, esp
    ; pushf
    ; pusha
    ; mov eax, [ebp + 8]
    ; mov [eax], esp
    ; mov eax, [ebp + 12]
    ; mov esp, [eax]
    ; popa
    ; mov ebp, [esp + 4]
    ; popf
    ; add esp, 4
    ; ret

; pushl	%ebp		/* Push ebp onto stack		*/
; 		movl	%esp,%ebp	/* Record current SP in ebp	*/
; 		pushfl			/* Push flags onto the stack	*/
; 		pushal			/* Push general regs. on stack	*/

; 		/* Save old segment registers here, if multiple allowed */

; 		movl	8(%ebp),%eax	/* Get mem location in which to	*/
; 					/*   save the old process's SP	*/
; 		movl	%esp,(%eax)	/* Save old process's SP	*/
; 		movl	12(%ebp),%eax	/* Get location from which to	*/
; 					/*   restore new process's SP	*/

; 		/* The next instruction switches from the old process's	*/
; 		/*   stack to the new process's stack.			*/

; 		movl	(%eax),%esp	/* Pop up new process's SP	*/

; 		/* Restore new seg. registers here, if multiple allowed */

; 		popal			/* Restore general registers	*/
; 		movl	4(%esp),%ebp	/* Pick up ebp before restoring	*/
; 					/*   interrupts			*/
; 		popfl			/* Restore interrupt mask	*/
; 		add	$4,%esp		/* Skip saved value of ebp	*/
; 		ret			/* Return to new process	*/

.fmt:
db "Addr of printB is %x\n", 0

.hello:
db 'hello world\n', 0
