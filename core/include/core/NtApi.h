#pragma once
#include <core/wobf/wobf.h>

namespace core {
	class NTAPI_EXPORT NtApi
	{
	public:
		static HANDLE WINAPI OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
			return OpenProcTh(dwDesiredAccess, bInheritHandle, dwProcessId, false);
		}
		static HANDLE WINAPI OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId) {
			return OpenProcTh(dwDesiredAccess, bInheritHandle, dwThreadId, true);
		}
		static bool WINAPI TerminateProcess(HANDLE hProcess, UINT uExitCode);
		static bool WINAPI TerminateThread(HANDLE hProcess, UINT uExitCode);
		static DWORD WINAPI SuspendThread(HANDLE hThread);
		static DWORD WINAPI ResumeThread(HANDLE hThread);
		static bool WINAPI CloseHandle(HANDLE h);
		static void WINAPI RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString);

		static HANDLE WINAPI CreateFileW(_In_ LPCWSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode,
			_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile
		);
		static bool WINAPI WriteFile(_In_ HANDLE hFile, _In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer, _In_ DWORD nNumberOfBytesToWrite,
			_Out_opt_ LPDWORD lpNumberOfBytesWritten, _Inout_opt_ LPOVERLAPPED lpOverlapped);

		static HANDLE WINAPI GetProcessHeap();
		static void WINAPI SetLastError(DWORD dwErrCode);
		static void WINAPI BaseSetLastNTError(NTSTATUS status);

	private:
		static HANDLE WINAPI OpenProcTh(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwId, bool isThread);
	};

	inline HANDLE WINAPI OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
	{
		return core::NtApi::OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	}

	inline HANDLE WINAPI OpenThread(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId)
	{
		return core::NtApi::OpenThread(dwDesiredAccess, bInheritHandle, dwThreadId);
	}

	inline bool WINAPI TerminateProcess(HANDLE hProcess, UINT uExitCode)
	{
		return core::NtApi::TerminateProcess(hProcess, uExitCode);
	}

	inline bool WINAPI TerminateThread(HANDLE hProcess, UINT uExitCode)
	{
		return core::NtApi::TerminateProcess(hProcess, uExitCode);
	}

	inline bool WINAPI SuspendThread(HANDLE hThread)
	{
		return core::NtApi::SuspendThread(hThread);
	}

	inline bool WINAPI ResumeThread(HANDLE hThread)
	{
		return core::NtApi::ResumeThread(hThread);
	}

	inline bool WINAPI CloseHandle(HANDLE h)
	{
		return core::NtApi::CloseHandle(h);
	}

	inline HANDLE WINAPI CreateFileW(_In_ LPCWSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode,
		_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile
	) {
		return core::NtApi::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
			lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	inline bool WINAPI WriteFile(_In_ HANDLE hFile, _In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer, _In_ DWORD nNumberOfBytesToWrite,
		_Out_opt_ LPDWORD lpNumberOfBytesWritten, _Inout_opt_ LPOVERLAPPED lpOverlapped) {
		return core::NtApi::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite,
			lpNumberOfBytesWritten, lpOverlapped);
	}

	inline HANDLE WINAPI GetProcessHeap()
	{
		return core::NtApi::GetProcessHeap();
	}

	inline void WINAPI RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString) {
		return core::NtApi::RtlInitUnicodeString(DestinationString, SourceString);
	}
}