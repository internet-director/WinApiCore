#pragma once
#include <core/wobf/wobf.h>

namespace core {
	class NTAPI_EXPORT NtApi
	{
	public:
		static HANDLE OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
		static HANDLE OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId);
		static bool TerminateProcess(HANDLE hProcess, UINT uExitCode);
		static DWORD SuspendThread(HANDLE hThread);
		static DWORD ResumeThread(HANDLE hThread);
		static bool CloseHandle(HANDLE h);

		static HANDLE GetProcessHeap();
	};

	inline HANDLE OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
	{
		return core::NtApi::OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	}

	inline HANDLE OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
	{
		return core::NtApi::OpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
	}

	inline bool TerminateProcess(HANDLE hProcess, UINT uExitCode)
	{
		return core::NtApi::TerminateProcess(hProcess, uExitCode);
	}

	inline bool SuspendThread(HANDLE hThread)
	{
		return core::NtApi::SuspendThread(hThread);
	}

	inline bool ResumeThread(HANDLE hThread)
	{
		return core::NtApi::ResumeThread(hThread);
	}

	inline bool CloseHandle(HANDLE h)
	{
		return core::NtApi::CloseHandle(h);
	}
	

	inline HANDLE GetProcessHeap()
	{
		return core::NtApi::GetProcessHeap();
	}
}