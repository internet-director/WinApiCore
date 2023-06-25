_DATA SEGMENT
call_number DQ 0
_DATA ENDS

.code

SystemCall proc
		mov r10, rcx
		mov eax, dword ptr[call_number]
		syscall
		ret
SystemCall endp

SetCallNumber proc
        mov call_number, rcx
        ret
SetCallNumber endp

end 