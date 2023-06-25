#pragma once
#include <core/wobf/wobf.h>

namespace core {
	class NTAPI_EXPORT NtApi
	{
	public:
		static HANDLE OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
			return OpenProcTh(dwDesiredAccess, bInheritHandle, dwProcessId, false);
		}
		static HANDLE OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId) {
			return OpenProcTh(dwDesiredAccess, bInheritHandle, dwThreadId, true);
		}
		static bool TerminateProcess(HANDLE hProcess, UINT uExitCode);
		static bool TerminateThread(HANDLE hProcess, UINT uExitCode);
		static DWORD SuspendThread(HANDLE hThread);
		static DWORD ResumeThread(HANDLE hThread);
		static bool CloseHandle(HANDLE h);
		static void RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString);

		static HANDLE GetProcessHeap();
		static HANDLE CreateFileW(_In_ LPCWSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode,
			_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile
		);

	private:
		static HANDLE OpenProcTh(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwId, bool isThread);
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

	inline bool TerminateThread(HANDLE hProcess, UINT uExitCode)
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


	inline HANDLE CreateFileW(_In_ LPCWSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode,
		_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile
	) {
		return core::NtApi::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
			lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	inline HANDLE GetProcessHeap()
	{
		return core::NtApi::GetProcessHeap();
	}

	inline void RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString) {
		return core::NtApi::RtlInitUnicodeString(DestinationString, SourceString);
	}
}