#include "pch.h"
#include <core/wobf/syscall.h>

DWORD syscallNumber = -1;

NTSTATUS __fastcall SystemCall()
{
	bool isWow64 = true;
	if (isWow64) {

	}
	__asm {
		mov eax, syscallNumber
		int 2eh
		retn
	}
}

NTSTATUS __fastcall NtAlwaysError()
{
	return NTSTATUS(-1);
}

void __fastcall SetCallNumber(DWORD num)
{
	syscallNumber = num;
}
