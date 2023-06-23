.code

asm_nt PROC
	mov r10, rcx
	mov eax, 15h
	syscall
	ret
asm_nt ENDP

asm_alloc PROC
	mov r10, rcx
	mov eax, 24h
	syscall
	ret
asm_alloc ENDP


END