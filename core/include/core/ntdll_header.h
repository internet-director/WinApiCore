#pragma once
#include <Windows.h>


#define FILE_ATTRIBUTE_VALID_FLAGS          0x00007fb7

typedef struct _FILE_FULL_EA_INFORMATION {
	ULONG NextEntryOffset;
	UCHAR Flags;
	UCHAR EaNameLength;
	USHORT EaValueLength;
	CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, * PFILE_FULL_EA_INFORMATION;

typedef struct _RTLP_CURDIR_REF {
	LONG RefCount;
	HANDLE DirectoryHandle;
} RTLP_CURDIR_REF, *PRTLP_CURDIR_REF;

typedef struct _RTL_RELATIVE_NAME_U {
	UNICODE_STRING RelativeName;
	HANDLE ContainingDirectory;
	PRTLP_CURDIR_REF CurDirRef;
} RTL_RELATIVE_NAME_U, * PRTL_RELATIVE_NAME_U;

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

NTSYSAPI NTSTATUS NTAPI NtTerminateThread(
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

NTSYSAPI NTSTATUS NTAPI NtAllocateVirtualMemory(
	IN HANDLE               ProcessHandle,
	IN OUT PVOID* BaseAddress,
	IN ULONG                ZeroBits,
	IN OUT PULONG           RegionSize,
	IN ULONG                AllocationType,
	IN ULONG                Protect
);
