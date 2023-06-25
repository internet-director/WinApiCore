#pragma once
#include <core/types.h>

extern "C" NTSTATUS __fastcall SystemCall();
extern "C" void __fastcall SetCallNumber(DWORD num);