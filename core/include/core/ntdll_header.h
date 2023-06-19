#pragma once
#include <Windows.h>

NTSYSAPI NTSTATUS NTAPI NtUnmapViewOfSection(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress
);

NTSYSAPI NTSTATUS NTAPI NtOpenProcess(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN CLIENT_ID* ClientId OPTIONAL
);

NTSYSAPI NTSTATUS NTAPI NtOpenThread(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN CLIENT_ID* ClientId OPTIONAL
);

NTSYSAPI NTSTATUS NTAPI NtTerminateProcess(
	IN HANDLE ProcessHandle OPTIONAL,
	IN NTSTATUS ExitStatus
);

NTSYSAPI NTSTATUS NTAPI NtSuspendThread(
	IN HANDLE ThreadHandle,
	OUT PULONG PreviousSuspendCount OPTIONAL
);

NTSYSAPI NTSTATUS NTAPI NtResumeThread(
	IN HANDLE ThreadHandle,
	OUT PULONG PreviousSuspendCount OPTIONAL
);