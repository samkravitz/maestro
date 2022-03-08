; prints a string to the screen
; si - pointer to string to be printed
; only use this in real mode
puts:
    lodsb
	or al, al		; test for NULL termination
	jz .done 
	mov ah, 0xe 
	int 10h
	jmp puts
.done:
	ret