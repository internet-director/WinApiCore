#include "pch.h"
#include "syscall.h"

DWORD syscallNumber;

NTSTATUS __fastcall SystemCall()
{
	return NTSTATUS();
}

NTSTATUS __fastcall NtAlwaysError()
{
	return NTSTATUS(-1);
}

void __fastcall SetCallNumber(DWORD num)
{
}
