#pragma once
#include <Windows.h>

NTSYSAPI
NTSTATUS
NTAPI
NtUnmapViewOfSection(
    IN HANDLE ProcessHandle,
    IN PVOID BaseAddress
);