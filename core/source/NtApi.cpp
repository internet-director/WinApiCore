#include "pch.h"
#include "NtApi.h"

namespace core {
	HANDLE NtApi::OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
	{
		NTSTATUS Status;
		OBJECT_ATTRIBUTES Obja;
		HANDLE Handle;
		CLIENT_ID ClientId;

		ClientId.UniqueThread = NULL;
		ClientId.UniqueProcess = LongToHandle(dwProcessId);

		InitializeObjectAttributes(
			&Obja,
			NULL,
			(bInheritHandle ? OBJ_INHERIT : 0),
			NULL,
			NULL
		);
		Status = API(NTDLL, NtOpenProcess)(
			&Handle,
			(ACCESS_MASK)dwDesiredAccess,
			&Obja,
			&ClientId
			);
		if (NT_SUCCESS(Status)) {
			return Handle;
		}
		return nullptr;
	}
	HANDLE NtApi::OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
	{
		NTSTATUS Status;
		OBJECT_ATTRIBUTES Obja;
		HANDLE Handle;
		CLIENT_ID ClientId;

		ClientId.UniqueThread = (HANDLE)LongToHandle(dwThreadId);
		ClientId.UniqueProcess = (HANDLE)NULL;

		InitializeObjectAttributes(
			&Obja,
			NULL,
			(bInheritHandle ? OBJ_INHERIT : 0),
			NULL,
			NULL
		);
		Status = API(NTDLL, NtOpenThread)(
			&Handle,
			(ACCESS_MASK)dwDesiredAccess,
			&Obja,
			&ClientId
		);
		if (NT_SUCCESS(Status)) {
			return Handle;
		}
		return nullptr;
	}
	bool NtApi::TerminateProcess(HANDLE hProcess, UINT uExitCode)
	{
		if (hProcess == nullptr) return false;
		NTSTATUS Status = API(NTDLL, NtTerminateProcess)(hProcess, (NTSTATUS)uExitCode);
		return NT_SUCCESS(Status);
	}
	DWORD NtApi::SuspendThread(HANDLE hThread)
	{
		NTSTATUS Status;
		DWORD PreviousSuspendCount = 0;

		Status = API(NTDLL, NtSuspendThread)(hThread, &PreviousSuspendCount);

		if (!NT_SUCCESS(Status)) {
			return (DWORD)-1;
		}
		return PreviousSuspendCount;
	}
	DWORD NtApi::ResumeThread(HANDLE hThread)
	{
		NTSTATUS Status;
		DWORD PreviousSuspendCount = 0;

		Status = API(NTDLL, NtResumeThread)(hThread, &PreviousSuspendCount);

		if (!NT_SUCCESS(Status)) {
			return (DWORD)-1;
		}
		return PreviousSuspendCount;
	}
	bool NtApi::CloseHandle(HANDLE h)
	{
		return NT_SUCCESS(API(NTDLL, NtClose)(h));
	}
	HANDLE NtApi::GetProcessHeap()
	{
		return reinterpret_cast<HANDLE>(size_t(core::_wobf.GetPEB()->ProcessParameters) - 10520);
	}
}