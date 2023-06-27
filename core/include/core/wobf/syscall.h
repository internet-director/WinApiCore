#pragma once
#include <core/config.h>

extern "C" {
	SYS_EXPORT NTSTATUS __fastcall SystemCall();
	SYS_EXPORT NTSTATUS __fastcall NtAlwaysError();
	SYS_EXPORT void __fastcall SetCallNumber(DWORD num);
}