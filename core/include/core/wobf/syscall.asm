_DATA SEGMENT
call_number DQ 0
_DATA ENDS

_TEXT SEGMENT

PUBLIC SystemCall
PUBLIC NtAlwaysError
PUBLIC SetCallNumber

SystemCall proc EXPORT 
		mov r10, rcx
		mov eax, dword ptr[call_number]
		syscall
		ret
SystemCall endp

NtAlwaysError proc EXPORT
        mov eax, -1
        ret
NtAlwaysError endp

SetCallNumber proc EXPORT
        mov call_number, rcx
        ret
SetCallNumber endp

_TEXT ENDS

end 