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
		static HANDLE CreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpName);
		static bool ReleaseMutex(HANDLE hMutex);
		static DWORD WINAPI WaitForSingleObject(_In_ HANDLE hHandle, _In_ DWORD dwMilliseconds);
		static DWORD WINAPI WaitForSingleObjectEx(_In_ HANDLE hHandle, _In_ DWORD dwMilliseconds, _In_ BOOL bAlertable);
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
		static HANDLE WINAPI GetCurrentProcess();
		static DWORD WINAPI GetLastError();
		static void WINAPI SetLastError(DWORD dwErrCode);
		static void WINAPI BaseSetLastNTError(NTSTATUS status);

		static bool WINAPI IsWow64Process(HANDLE hProcess, PBOOL Wow64Process);

		static LPSYSTEM_INFO WINAPI GetSystemInfo();

	private:
		static HANDLE WINAPI OpenProcTh(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwId, bool isThread);
		static void GetSystemInfoInternal(
			IN wtype::PSYSTEM_BASIC_INFORMATION BasicInfo,
			IN wtype::PSYSTEM_PROCESSOR_INFORMATION ProcessorInfo,
			OUT LPSYSTEM_INFO lpSystemInfo
		);
		static PLARGE_INTEGER BaseFormatTimeOut(PLARGE_INTEGER TimeOut, DWORD Milliseconds);
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

	inline HANDLE WINAPI GetCurrentProcess()
	{
		return core::NtApi::GetCurrentProcess();
	}

	inline bool WINAPI IsWow64Process(HANDLE hProcess, PBOOL Wow64Process) {
		return core::NtApi::IsWow64Process(hProcess, Wow64Process);
	}

	inline DWORD WINAPI GetLastError()
	{
		return core::NtApi::GetLastError();
	}

	inline void WINAPI SetLastError(DWORD dwErrCode)
	{
		return core::NtApi::SetLastError(dwErrCode);
	}

	inline void WINAPI BaseSetLastNTError(NTSTATUS status)
	{
		return core::NtApi::BaseSetLastNTError(status);
	}

	inline void WINAPI RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString) {
		return core::NtApi::RtlInitUnicodeString(DestinationString, SourceString);
	}

	inline LPSYSTEM_INFO WINAPI GetSystemInfo() {
		return core::NtApi::GetSystemInfo();
	}

	inline static DWORD WINAPI WaitForSingleObject(_In_ HANDLE hHandle, _In_ DWORD dwMilliseconds) {
		return core::NtApi::WaitForSingleObject(hHandle, dwMilliseconds);
	}

	inline static HANDLE CreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpName) {
		return core::NtApi::CreateMutexW(lpMutexAttributes, bInitialOwner, lpName);
	}

	inline static bool ReleaseMutex(HANDLE hMutex) {
		return core::NtApi::ReleaseMutex(hMutex);
	}
}