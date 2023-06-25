#include "pch.h"
#include "syscall.h"

DWORD syscallNumber;

NTSTATUS __fastcall SystemCall()
{
	return NTSTATUS();
}

void __fastcall SetCallNumber(DWORD num)
{
}
